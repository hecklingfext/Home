#!/bin/sh
#
# The most complicated, convoluted, complex installation
# in the history of the Internet.
#
echo -n "Copying libnpfbook_1_0_3.so to $HOME/.mozilla/plugins/libnpfbook_1_0_3.so..."
mkdir -p "$HOME"/.mozilla/plugins
cp libnpfbook_1_0_3.so "$HOME"/.mozilla/plugins/libnpfbook_1_0_3.so

if [ $? -eq 0 ]; then
echo " done."
else
echo " failed."
fi

