#! /bin/bash
#this script installs the extensions necessary for this simulator to work

if [ -z "$R3BROOTPATH" ]; then
	echo "Problem: can't find R3BRoot."
	exit
fi

ln -sf -t $R3BROOTPATH/lib/ $PWD/lib/*.so
ln -sf -t $R3BROOTPATH/lib/ $( find $PWD -name '*.pcm' )
cp $( find . -name 'R3B*.h' ) $R3BROOTPATH/include/
