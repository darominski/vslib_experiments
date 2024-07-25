# Vitis workspace creation procedure
proc CreateWorkspace {workspace_dir} {
    setws -switch $workspace_dir
}

# Patches the BSP of the platform project so it can work with the bmboot
proc PatchPlatform {root_dir workspace_dir plat_name cpu_id} {
    cd $plat_name
    exec patch -p1 < $root_dir/inputs/bsp_cpu${cpu_id}.patch
    cd ..
}

# Vitis platform project creation procedure
proc CreatePlatformProject {root_dir xsa_dir proc_name cpu_id workspace_dir} {
    set plat_name "a53_${cpu_id}"
    set project_domain "cortex_a53_${cpu_id}"

    # The main command to create the platform based on either
    # DI/OT v2 board or Kria K26 specification.
    # It uses fixed-location XSA file, standalone (bare-metal) operating system,
    # places the FSBL on the _0 processor (by default), and 64bit architecture (default),
    # and puts the project in its own subdirectory.
    # The FSBL target and architecture can be specified in V2023.1 by:
    # -arch {64-bit} -fsbl-target {psu_cortexa53_0}
    #
    # On DIOTv2 there are only 2 Cortex A53 cores, the VSlib takes the 2nd (_1) bare metal one.
    # On Kria K26 and and the future FGC4, there are 4 A53 cores, it will be the 4th (_3).
    if (![file exists $plat_name]) {
	    # platform project is only generated once even if there are multiple application projects
	    platform create -name $plat_name\
	        -hw $xsa_dir\
	        -proc $proc_name\
	        -os "standalone"\
	        -out $workspace_dir

	    # necessary for the platform project to appear in Vitis:
	    importproject $workspace_dir
	    # -domains ensures that only the specified domain will be built
	    platform generate -domains "standalone_domain"
	    PatchPlatform $root_dir $workspace_dir $plat_name $cpu_id
    }
    platform active $plat_name
}

# Vitis custom application project creation procedure
proc CreateApplicationProject {app_name proc_name source_dir workspace_dir root_dir bmboot_dir bmboot_binary_dir} {
    # creates empty application project connected to the active bare-metal domain
    app create -name $app_name\
	-proc $proc_name\
	-domain "standalone_domain"\
	-os "standalone"\
	-lang "C++"\
	-template "Empty Application"

    # fills the project with the code from the cloned repository
    importsources -name $app_name -path $source_dir -soft-link

    # configures correctly the location of the include files for both Release and Debug builds
    set builds {release debug}
    foreach build $builds {
		app config -name $app_name -set build-config $build
		app config -name $app_name -add "include-path" $source_dir/../vslib/background/inc
		app config -name $app_name -add "include-path" $source_dir/../vslib/components/inc
		app config -name $app_name -add "include-path" $source_dir/../vslib/parameters/inc
		app config -name $app_name -add "include-path" $source_dir/../vslib/interrupts/inc
		app config -name $app_name -add "include-path" $source_dir/../vslib/utils/inc
		app config -name $app_name -add "include-path" $source_dir/../hal/inc
		app config -name $app_name -add "include-path" $source_dir/../utils
		app config -name $app_name -add "include-path" $libraries_dir/json-3.11.2
		app config -name $app_name -add "include-path" $libraries_dir/magic_enum-0.9.3
		app config -name $app_name -add "include-path" $libraries_dir/fmt-8.0.1/include
		app config -name $app_name -add "include-path" $libraries_dir/json-schema-validator-2.2.0/src/
		app config -name $app_name -add "include-path" $workspace_dir/build-vslib/_deps/nlohmann_json-src/include
		app config -name $app_name -add "compiler-misc" "-std=c++20"
		app config -name $app_name -set "linker-misc" "-Wl,--undefined=_close -Wl,--undefined=_fstat -Wl,--undefined=_isatty -Wl,--undefined=_lseek -Wl,--undefined=_read -Wl,--undefined=_write -Wl,--start-group,-lxil,-lgcc,-lc,-lstdc++,--end-group -specs=nosys.specs"
		app config -name $app_name -add "include-path" $bmboot_dir
		app config -name $app_name -add "library-search-path" $bmboot_binary_dir
		app config -name $app_name -add "library-search-path" $bmboot_binary_dir/monitor_zynqmp-prefix/src/monitor_zynqmp-build
		app config -name $app_name -add "library-search-path" $vslib_build_dir
		app config -name $app_name -add "library-search-path" $vslib_build_dir/fmt
		app config -name $app_name -add "library-search-path" $vslib_build_dir/json-schema-validator
		app config -name $app_name -add "libraries" fmt
		app config -name $app_name -add "libraries" nlohmann_json_schema_validator
		app config -name $app_name -add "libraries" vslib
		app config -name $app_name -add "libraries" bmboot_payload_runtime
		app config -name $app_name -set "linker-script" $root_dir/inputs/lscript.ld
    }

    # builds the application
    app build -name $app_name
}

set root_dir [lindex $argv 0]
set workspace_dir [lindex $argv 1]
set xsa_dir [lindex $argv 2]
set source_dir [lindex $argv 3]
set app_name [lindex $argv 4]
set cpu_id [lindex $argv 5]
set bmboot_include_dir [lindex $argv 6]
set bmboot_binary_dir [lindex $argv 7]
set libraries_dir [lindex $argv 8]
set vslib_build_dir [lindex $argv 9]

set proc_name "psu_cortexa53_$cpu_id"

# Vitis workspace creation
CreateWorkspace $workspace_dir
cd $workspace_dir

# Platform project creation
CreatePlatformProject $root_dir $xsa_dir $proc_name $cpu_id $workspace_dir

# Application project creation and build
CreateApplicationProject $app_name $proc_name $source_dir $workspace_dir $root_dir $bmboot_include_dir $bmboot_binary_dir $libraries_dir $vslib_build_dir
