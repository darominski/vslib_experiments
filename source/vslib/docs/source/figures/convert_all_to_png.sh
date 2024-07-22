#!/bin/bash

ls *eps | { eps_name=$(< /dev/stdin); gs -dSAFER -dEPSCrop -r600 -sDEVICE=pngalpha -o ${eps_name%.eps*}.png ${eps_name}; }
ls *drawio | { drawio_name=$(< /dev/stdin); drawio -x -f png -s 1 ${drawio_name}; }
