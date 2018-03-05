#! /bin/bash

#tiny script to install (and uninstall) the script and the program in /usr/local/bin/

if [ -z "$1" ]; then
	if ! [ -h /usr/local/bin/sbkg ]; then
		ln -s $PWD/sbkg /usr/local/bin/
	else
		echo "install.sh: evtmux already installed."
	fi
elif [ "$1" == "-u" ]; then
	rm -f /usr/local/bin/sbkg
fi

