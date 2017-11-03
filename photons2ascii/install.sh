#tiny shell script that installs the libraries and the programs

#core
if [ -L /usr/local/lib/libp2a.so ]; then
	echo "Library \"libxb_core\" already installed."
else
	ln -s $PWD/lib/libp2a.so /usr/local/lib/
	ln -s $PWD/lib/libp2a.so /usr/lib/
	ln -s $PWD/lib/libp2a.so /usr/lib64/
fi

#headers
for f in $( ls include/*.h ); do
	if [ ! -L /usr/local/include/$f ]; then
		ln -s $PWD/include/$f /usr/local/include
		echo $f
	fi
done

#programs
if [ -L /usr/local/bin/p2a ]; then
	echo "Program \"p2a\" already installed."
else
	ln -s $PWD/p2a /usr/local/bin/
fi
