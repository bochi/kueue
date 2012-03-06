#!/bin/bash
#
# mkdoc.sh - Create HTML documentation for kueue
# This requires suse-xsl-stylesheets to be installed
# Available at Documentation:/Tools in the OBS
#

xsltproc --output index.html /usr/share/xml/docbook/stylesheet/suse/xhtml/chunk.xsl index.docbook
cp /usr/share/doc/manual/opensuse-manuals_en/susebooks.css html

echo "" >> html/susebooks.css
echo "body:after {" >> html/susebooks.css
echo "content:" >> html/susebooks.css
for i in images/* ; do echo "url($i)" >> html/susebooks.css; done
echo ";" >> html/susebooks.css
echo "display: none;" >> html/susebooks.css
echo "}" >> html/susebooks.css

scp -r html/* root@kueue.hwlab.suse.de:/srv/www/htdocs/doc
scp -r images root@kueue.hwlab.suse.de:/srv/www/htdocs/doc
scp -r scripts root@kueue.hwlab.suse.de:/srv/www/htdocs/doc
