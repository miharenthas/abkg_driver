#! /bin/bash

#tiny script to install (and uninstall) the script and the program in /usr/local/bin/

if [ -z "$1" ]; then
	if ! [ -h /usr/local/bin/evtmux ]; then
		ln -s $PWD/evtmux /usr/local/bin/
	else
		echo "install.sh: evtmux already installed."
	fi
	if ! [ -h /usr/local/bin/evtfunnel ]; then
		ln -s $PWD/evtfunnel /usr/local/bin/
	else
		echo "install.sh: evtfunnel already installed."
	fi
	if ! [ -h /usr/local/bin/multisim ]; then
		ln -s $PWD/multisim.sh /usr/local/bin/multisim
	else
		echo "install.sh: multisim already installed."
	fi
elif [ "$1" == "-u" ]; then
	rm -f /usr/local/bin/evtmux
	rm -f /usr/local/bin/evtfunnel
	rm -f /usr/local/bin/multisim
fi

