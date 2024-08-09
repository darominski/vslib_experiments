#!/bin/bash

find . -name '*.eps' -exec sh -c 'gs -dSAFER -dEPSCrop -r600 -sDEVICE=pngalpha -o "${0%.eps}.png" "$0"' {} \;