"""
mmtool -- FGC4 memory map authoring utility

mmtool uses a central source of truth to generate files in disparate formats.
This is done by taking a _memory map_ definition together with an _output specification_
("spec" for short).
The memory map specifies the addresses and sizes of memory regions, while the spec decides
how the outputs should be rendered.

To reduce hard-coding, sub-regions can have their addresses assigned automatically.

As for the outputs, the tool can generate C/C++ header files with a subset of the definitions,
or it can make substitutions into a template in an arbitrary format.


Memory map format:

- INI-like syntax, sections ~ memory regions
- for top-level regions, specify range using `start` and either `end` (exclusive) or `size`
- regions can be nested using the `in` keyword
    - do not hard-code the start, use `size` and optionally `align`
- sizes can use 'k', 'M', 'G' suffixes. they are power-of-2-based rather than SI

Example:

    [fgc4_shmem]
    start = 0x8_0000_0000
    end = 0x8_8000_0000

    [bmboot.cpu1_monitor]
    in = fgc4_shmem
    align = 4k
    size = 64k

    [bmboot.cpu2_monitor]
    in = fgc4_shmem
    align = 4k
    size = 64k


Output specification format:

- INI-like syntax

- `generate` section: generate C/C++ header
    - `path`: output path, realtive to spec file
    - `regions`: multi-line string, specifies names of regions to include. can use wildcards (see example)

- `substitute` section: substitute placeholders of the form {{region_name.ADDRESS}} and {{region_name.ADDRESS}}
    - `in`: input path
    - `out`: output path
    - `aliases`: optional, multi-line string; can specify space-delimited pairs of new_name, old_name

- Since the syntax parser requires section names to be unique, you can put arbitrary suffixes to meet this requirement.

Example:

    [generate]
    path = src/bmboot_memmap.hpp

    regions =
      bmboot.*

    [substitute:monitor-cpu1]
    in = src/executor/monitor/monitor.ld.in
    out = src/executor/monitor/monitor_cpu1.ld
    aliases =
      bmboot.cpuN_monitor bmboot.cpu1_monitor


Review:

Perhaps the format should have just been JSON. It's annoying to hand-edit, but allows unlimited
hierarchy.

"""

import argparse
import configparser
from dataclasses import dataclass
import fnmatch
from pathlib import Path
from typing import Optional


@dataclass
class Region:
    name: str
    parent: Optional["Region"]
    start: int
    size: int
    alignment: Optional[int]
    used: int


def align(value, alignment):
    excess = value % alignment
    if excess:
        return value + (alignment - excess)
    else:
        return value


def parse_address(spec):
    return int(spec, base=0)


def parse_size(spec):
    mul = 1
    if spec.endswith("k"):
        spec = spec.removesuffix("k")
        mul = 1024
    elif spec.endswith("M"):
        spec = spec.removesuffix("M")
        mul = 1024 ** 2

    return int(spec, base=0) * mul


def parse_memory_map(f):
    config = configparser.ConfigParser(default_section=None)
    config.read_file(f)

    # print(*config.items())

    regions = {}

    for name, section in config.items():
        if name is None:
            assert len(section) == 0
            continue

        if "in" in section:
            parent = regions[section["in"]]
        else:
            parent = None

        alignment = parse_size(section.get("align", "1"))

        if "start" in section:
            assert parent is None       # this case not implemented
            assert "align" not in section

            start = parse_address(section["start"])
        else:
            if parent is None:
                raise Exception(f"{name}: Must specify either 'in' or 'start'")

            start = align(parent.start + parent.used, alignment)

        if "size" in section:
            size = parse_size(section["size"])
        elif "end" in section:
            end = parse_address(section["end"])
            assert end >= start
            size = end - start
        else:
            raise Exception(f"{name}: Neither 'size' nor 'end' specified")

        if parent is not None:
            parent.used = start + size - parent.start

            if parent.used > parent.size:
                raise Exception(f"{parent.name}: Region overflowed after {name} with {parent.used} > {parent.size}")

        regions[name] = Region(name=name,
                            parent=parent,
                            start=start,
                            size=size,
                            alignment=alignment,
                            used=0)
    return regions


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("input", type=Path)
    parser.add_argument("output_spec", type=Path, nargs="+")
    args = parser.parse_args()

    with open(args.input) as f:
        regions = parse_memory_map(f)

    # for r in regions.values():
    #     print(r)

    for output_spec in args.output_spec:
        config = configparser.ConfigParser(default_section=None)

        with open(output_spec) as f:
            config.read_file(f)

        # print(*config.items())

        for name, options in config.items():
            if not name:
                continue

            if name.startswith("generate"):
                output_path = output_spec.parent / options["path"]

                if output_path.suffix not in {".h", ".hpp"}:
                    raise Exception(f"{output_path}: Don't know how to produce a file in this format. Supported formats: .h, .hpp")

                with open(output_path, "wt") as f:
                    f.write(f"#pragma once\n\n/* This file was automatically generated from {args.input.name}. Manual editing is discouraged. */\n\n")

                    all_region_names = set(regions.keys())
                    matched_region_names = set()

                    # expand wildcards
                    for name in options["regions"].split():
                        matches = fnmatch.filter(all_region_names, name.strip())
                        if not matches:
                            raise Exception(f"{output_spec}: No match for region '{name.strip()}'")
                        matched_region_names.update(matches)

                    for name in sorted(matched_region_names):
                        r = regions[name]
                        name_sanitized = r.name.replace(".", "_")

                        def define(symbol, value):
                            f.write(f"#define {symbol.ljust(32)} {value}\n")

                        define(f"{name_sanitized}_ADDRESS", f"0x{r.start:08X}")
                        define(f"{name_sanitized}_SIZE", f"0x{r.size:08X}")
            elif name.startswith("substitute"):
                input_path = output_spec.parent / options["in"]
                output_path = output_spec.parent / options["out"]
                aliases = options.get("aliases", "")

                aliased_regions = {**regions}

                for row in aliases.split("\n"):
                    if not row: continue
                    to, from_ = row.split()
                    aliased_regions[to] = aliased_regions[from_]

                contents = input_path.read_text()

                # this is HYPER INEFFICIENT but we hyper don'tcare
                for name, r in aliased_regions.items():
                    contents = contents.replace("{{" + name + ".ADDRESS}}", f"0x{r.start:08X}")
                    contents = contents.replace("{{" + name + ".SIZE}}", f"0x{r.size:08X}")

                output_path.write_text(contents)
