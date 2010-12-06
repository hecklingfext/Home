#!/bin/sh

VERS=0.2.9

ls | grep -v autom4te | grep -v CVS | sed s:^:3ddesktop-$VERS/: >MANIFEST
cd ..
ln -s 3ddesktop 3ddesktop-$VERS
tar -czvhf 3ddesktop-$VERS.tar.gz `cat 3ddesktop/MANIFEST`
rm 3ddesktop-$VERS
cd 3ddesktop
