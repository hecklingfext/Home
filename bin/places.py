#!/usr/bin/env python
# Dynamic places menu generator for openbox
# Uses a list of parent directories to provide a menu of subdirectories
# After removing any entries found in an ignore list

import glob
import fnmatch
from os.path import expanduser

# File manager you want to use to open directories
manager = "pcmanfm"

# User home directory. If you hard code this location instead of relying on
# expanduser() then you can remove its import above
home = expanduser('~')

# List of directories whose subdirectories we want to display
dirs = [home, '/media']

# List of directories to ignore
# Shell-style wildcards as used by fnmatch are supported
ignore = ['/media/cdrom0']

# Our list of menu items
# If you want to add single directories to the menu without including 
# all of their subdirs then put them in here
items = []

# Iterate through dirs 
for dir in dirs:
    # Get a list of subdirectories for each dir
    subdirs = glob.glob(dir + '/*/')
    # Alphabetise subdirs. Sorting here is less efficient but preserves 
    # directory order specified above. If this doesn't matter to you then
    # remove this line and uncomment "items.sort(key=str.lower)" below
    subdirs.sort(key=str.lower)
    # Append each subdir to the items list
    for sub in subdirs:
        # Replace /home/user with ~ before appending to items (looks better)
        sub = sub.replace(home, '~')
        # Strip trailing / characters
        sub = sub.rstrip('/')
        items.append(sub)

# Iterate through ignore list and remove matches from items
for i in ignore:
    matches = fnmatch.filter(items, i)
    for m in matches:
        try:
            items.remove(m)
        except ValueError:
            pass

# Alphabetise directory list. Read comment for sort() above before uncommenting
#items.sort(key=str.lower)

# Output xml for the openbox menu
print '<openbox_pipe_menu>'
# Each item becomes a menu entry
for i in items:
    print '<item label="' + i + '">'
    print '<action name="Execute">'
    print '<execute>'
    print manager + " " + i
    print '</execute>'
    print '</action>'
    print '</item>'
print '</openbox_pipe_menu>'
