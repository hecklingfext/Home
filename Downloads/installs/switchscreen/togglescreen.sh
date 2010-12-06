#!/bin/bash

STOREFILE=~/.togglescreen
SWITCHSCREEN=switchscreen


CURRENT="`$SWITCHSCREEN -P`"
CURSCR=`echo "$CURRENT" | gawk '{ print $2 }'`
CURPOS=`echo "$CURRENT" | gawk '{ print $4 }'`

echo $CURPOS > $STOREFILE.$CURSCR

if [ "$CURSCR" = "0" ]; then
    NEWSCR=1
else
    NEWSCR=0
fi

NEWPOS="`cat $STOREFILE.$NEWSCR 2>/dev/null`"
if [ "$NEWPOS" ]; then
    POSOPT="-c $NEWPOS"
else
    POSOPT=""
fi

$SWITCHSCREEN -q $POSOPT $NEWSCR
