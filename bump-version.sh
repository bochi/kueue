#!/bin/bash

if [ "$1" = "" ]; then

    echo "Usage: bump-version.sh VERSION";
    exit;

fi

sed "s/git/$1/g" CMakeLists.txt > tmp
mv tmp CMakeLists.txt

sed "s/git/$1/g" main.cpp > tmp
mv tmp main.cpp

sed "s/git/$1/g" kueue.desktop > tmp
mv tmp kueue.desktop

sed "s/git/$1/g" packaging/rpm/kueue.spec > tmp
mv tmp packaging/rpm/kueue.spec

sed "s/git/$1/g" packaging/debian/kueue.dsc > tmp
mv tmp packaging/debian/kueue.dsc

sed "s/git/$1/g" packaging/debian/debian.control > tmp
mv tmp packaging/debian/debian.control

sed "s/git/$1/g" packaging/osx/kueue.plist > tmp
mv tmp packaging/osx/kueue.plist

echo Done, Version bumped to $1.
