#!/bin/bash

# Rozbaleni archivu
unzip src.zip

# Kompilace knihovny Qwt
qmake ./src/qwt/qwt.pro -o Makefile
make -C src/qwt

# Nastaveni promenne prostredi $QWT
PART1=`pwd`
PART2="/src/qwt"
export QWT=$PART1$PART2

# Zkopirovani dynamickych knihoven do /usr/lib
cp ./src/qwt/lib/* /usr/lib/

# Kompilace programu
cd src
qmake
cd ..
make -C src

# Presun slozek
mv src/example_tasks/ ./
mv src/example_solutions/ ./
mv src/configuration.txt ./
mv src/configuration.xml ./

# Sebedestrukce
rm src.zip
rm install.sh
