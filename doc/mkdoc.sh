#!/bin/bash
#
# mkdoc.sh - Create HTML documentation for kueue
# This requires suse-xsl-stylesheets to be installed
# Available at Documentation:/Tools in the OBS
#

xsltproc --output index.html /usr/share/xml/docbook/stylesheet/suse/xhtml/chunk.xsl index.docbook
cp /usr/share/doc/manual/opensuse-manuals_en/susebooks.css html

echo "<RCC>" > ../doc.qrc
echo "<qresource prefix=\"help\">" >> ../doc.qrc

cd html

for i in $(ls *.html *.css); do

    echo "<file alias=\"$i\">doc/html/$i</file>" >> ../../doc.qrc

done

cd ../images

for i in $(ls *.png); do 

    echo "<file alias=\"images/$i\">doc/images/$i</file>" >> ../../doc.qrc

done

cd ../scripts

for i in $(ls kueue*); do

    echo "<file alias=\"scripts/$i\">doc/scripts/$i</file>" >> ../../doc.qrc

done

cd ../..

echo "</qresource>" >> doc.qrc
echo "</RCC>" >> doc.qrc

