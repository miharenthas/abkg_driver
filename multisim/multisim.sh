#! /bin/bash

#this script runs the simulation of one ASCII event file and launches its simulation
#in parallel, with the program and options indicated. In the case of this toolkit
#such a program is "sbkg", but the name of the program is taken from the command line

#One has to follow this approach because ROOT and OpenMP are not compatible
#at least until I can come around and rewrite the code for MPI in an acceptable
#manner. The unelegant (and the one that will require attention even in MPI) is
#the output in multiple files.

#=====================================================================================
#global variables

PROGRAM="" #the program that will be used
CMD="" #the command line for the program
INPUT_FILE="" #the input file that will eventually used
OUTPUT_FILE="" #need to intercept the output file as well, sadly
NB_ONLINE_CPUs=$( nproc ) #number of online processors

#flags
file_flag=0 #true if we are reading from a file and not from stdin
compression_flag=0 #true if we are dealing with a compressed file

#=====================================================================================
#parsing and environment setup
#function "msim_parse_cmd_line()" parses the options and sets the variable (evtl.)

#-------------------------------------------------------------------------------------
#organize the command line
msim_parse_cmd_line(){
	#check if $1 is '-F' or '-C', in that case we have an input file, compressed
	#in the latter case
	if [ $1 == "-F" ]; then
		shift
		INPUT_FILE=$1
		file_flag=1
	elif [ $1 == "-C" ]; then
		shift
		INPUT_FILE=$1
		file_flag=1
		compression_flag=1
	fi
	
	#now comes the name of the program
	PROGRAM=$1
	shift
	
	#now the options are parsed
	while [ $# -gt 0 ]; do
		#intercept the output file's name
		#save it and remove it from the command line
		if [ $1 == "-o" ] || [ $1 == "--output-file" ]; then
			shift
			OUTPUT_FILE=$1
			shift
		fi
		
		#just piling on...
		CMD=$CMD" "$1
		shift
	done
}

#====================================================================================
#running the simulation in parallel
#function "msim_run_sim()" runs the simulation in parallel. It opens the file and then
#         reads and distributes parts of it to some programs

#------------------------------------------------------------------------------------
#launch all the simulations
msim_run_sim(){
	nb_active_jobs=0
	
	#first thing to do: prepare the execution:
	#nproc pipes will be created and through those
	#we're going to feed the simulators.
	while [ $nb_active_jobs -le $NB_ONLINE_CPUs ]; do
		#open a (named) pipe (hidden, for aesthetics)
		current_pipe=$( printf ".msim_%02d" $nb_active_jobs )
		mkfifo $current_pipe
		
		#make the name of the output file
		current_ofile=$( printf "proc_%02d_" $nb_active_jobs )$OUTPUT_FILE
		
		$PROGRAM $current_pipe $CMD" -o "$current_ofile &
		
		nb_active_jobs=$( pgrep -P $$ -c "$PROGRAM" )
	done
	
	#now, let's unleash the event demuxer program
	if [ $file_flag -eq 1 ]; then
		if [ $compression_flag -eq 1 ]; then
			mkfifo .msim_src
			bzcat $INPUT_FILE > .msim_src
			evtmux .msim_src .msim_??
		else
			evtmux $INPUT_FILE .msim_??
		fi
	else
		mkfifo .msim_src
		3>.msim_src
		0>&3
		evtmux .msim_src .msim_??
	fi
	
	#do job control
	msim_job_control "$PROGRAM"
	
	#cleanup all the pipes
	rm -f .msim_*
}

#------------------------------------------------------------------------------------
#do job control (basically copied from abkg_driver, but here is an
#error catchy waiting instead)
msim_job_control(){
	while [ $nb_active_jobs -ge 0 ]; do
		sleep 1
		nb_active_jobs=$( pgrep -P $$ -c "$1" )
		
		#because, when Geant3 crashes, we get stuck on gdb for ages
		#here is a check we will also perform. It's cumbersome
		#because of the way ROOT launches gdb on crash.
		nb_failed=$( pgrep -c "gdb" )
		if [ $nb_failed -gt 0 ]; then
			for a_job in $( pgrep -P $$ "$1" -d " " ); do
				if [ $( pgrep -P $a_job -c "sh" ) -gt 0 ]; then
					kill -SIGKILL $( pgrep -P $( pgrep -P $( pgrep -P $a_job "sh" ) "gdb-backtrace" ) "gdb" )
					
					echo "WARNING: root process $a_job crashed and was killed. See log for details."
					echo "         You may have to run this particular job again..."
				fi
			done
		fi
	done
}

#------------------------------------------------------------------------------------
#da main

