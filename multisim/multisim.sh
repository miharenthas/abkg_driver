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
OUTPUT_FILE="r3bsim.root" #need to intercept the output file as well, sadly
PAR_FILE="r3bpar.root" #need to intercept also the parameter file (avoid mangling)
NB_ONLINE_CPUs=$( nproc ) #number of online processors
VERBOSE_FLAG=0 #the verbose flag, causes the program to print something

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
	while [[ $1 == -* ]]; do
		case $1 in
			-F )
				shift
				INPUT_FILE=$1
				file_flag=1
				shift
				;;
			-C )
				shift
				INPUT_FILE=$1
				file_flag=1
				compression_flag=1
				shift
				;;
			-h | --help )
				cat doc/multisim_help
				;;
			-v | --verbose )
				VERBOSE_FLAG=1
				shift
				;;
		esac
	done
	
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
		#intercept also the parameter file
		if [ $1 == "-p" ] || [ $1 == "--parameter-file" ]; then
			shift
			PAR_FILE=$1
			shift
		fi
		
		#just piling on...
		CMD=$CMD" "$1
		shift
	done
}

#=====================================================================================
#running the simulation in parallel
#function "msim_run_sim()" runs the simulation in parallel. It opens the file and then
#         reads and distributes parts of it to some programs

#-------------------------------------------------------------------------------------
#launch all the simulations
msim_run_sim(){
	nb_active_jobs=0
	
	if [ $VERBOSE_FLAG -eq 1 ]; then
			logfile="msim_log_"$( date +%F-%H-%M-%S )
			echo "Launching: "
	fi
	
	#first thing to do: prepare the execution:
	#nproc pipes will be created and through those
	#we're going to feed the simulators.
	while [ $nb_active_jobs -lt $NB_ONLINE_CPUs ]; do
		#open a (named) pipe (hidden, for aesthetics)
		current_pipe=$( printf ".msim_%02d" $nb_active_jobs )
		if ! [ -p $current_pipe ]; then mkfifo $current_pipe; fi
		
		#make the name of the output file
		current_ofile=$OUTPUT_FILE$( printf ".proc_%02d" $nb_active_jobs )
		current_pfile=""
		if [ $nb_active_jobs -eq 0 ]; then
			current_pfile=$PAR_FILE;
		else
			current_pfile=/dev/null
		fi
		
		if [ $VERBOSE_FLAG -eq 0 ]; then
			$PROGRAM $current_pipe $CMD -o $current_ofile -p $current_pfile 2>/dev/null 1>&2 &
		elif [ $VERBOSE_FLAG -eq 1 ]; then
			$PROGRAM $current_pipe $CMD -o $current_ofile -p $current_pfile 2>>$logfile 1>&2 &
			echo -e "\t"$PROGRAM $current_pipe $CMD -o $current_ofile -p $current_pfile
		fi
		
		nb_active_jobs=$( pgrep -f -P $$ -c "$PROGRAM" )
	done

	#now, let's unleash the event demuxer program
	if ! [ -p ".msim_src" ]; then mkfifo .msim_src; fi
	if [ $file_flag -eq 1 ]; then
		if [ $compression_flag -eq 1 ]; then
			evtmux .msim_src .msim_??
			bzcat $INPUT_FILE > .msim_src
		else
			evtmux $INPUT_FILE .msim_??
		fi
	else
		evtmux -- .msim_??
	fi
	
	#do job control
	if [ $VERBOSE_FLAG -eq 1 ]; then
		echo -n "Running: "
	fi
	
	msim_job_control "$PROGRAM"
	
	if [ $VERBOSE_FLAG -eq 1 ]; then
		echo " done."
	fi
	
	#cleanup all the pipes
	rm -f .msim_*
}

#=====================================================================================
#auxiliary functions:
#function "msim_job_control" does some very basic job control (hunts down and kills
#         the debugger launched by ROOT).
#function "msim_join_root_files" does the stiching of root files, if the sum is less
#         than what hadd can handle (lifted from abkg_driver)

#-------------------------------------------------------------------------------------
#do job control (basically copied from abkg_driver, but here is an
#error catchy waiting instead)
msim_job_control(){
	#useful for stylish running viZ
	seconds=0
	echo -n "-"
	
	while [ $nb_active_jobs -gt 0 ]; do
		sleep 1
		
		if [ $VERBOSE_FLAG -eq 1 ]; then
			echo -ne "\b"
			case $(( seconds%4 )) in
				0 ) echo -n "-"
				        ;;
				1 ) echo -n "\\"
				    ;;
				2 ) echo -n "|"
				    ;;
				3 ) echo -n "/"
				    ;;
			esac
			seconds=$(($seconds+1))
		fi
		
		nb_active_jobs=$( pgrep -f -P $$ -c "$1" )
		
		#because, when Geant3 crashes, we get stuck on gdb for ages
		#here is a check we will also perform. It's cumbersome
		#because of the way ROOT launches gdb on crash.
		nb_failed=$( pgrep -c "gdb" )
		if [ $nb_failed -gt 0 ]; then
			for a_job in $( pgrep -f -P $$ "$1" -d " " ); do
				if [ $( pgrep -P $a_job -c "sh" ) -gt 0 ]; then
					kill -SIGKILL $( pgrep -P $( pgrep -P $( pgrep -P $a_job "sh" ) "gdb-backtrace" ) "gdb" )
					
					echo "multisim: warning: root process $a_job crashed and was killed. See log for details." >&2
					echo "                   You may have to run this particular job again..." >&2
				fi
			done
		fi
	done
}

#-------------------------------------------------------------------------------------
#this function stitches root files together
#the last input is the output file name
msim_join_root_files(){
	tmp_files=""
	while [ "$2" != "" ]; do
		tmp_files=$tmp_files" "$1
		shift;
	done
	stitched_file=$1
	
	#if the total size is more than 99GiB hadd will fail. Test for that
	#and proceed only if the simulation is reasonably small.
	if [ $( du -c $tmp_files | grep total | sed "s/total//g" ) -le 103809024 ]; then
	
		hadd -v 0 -f $stitched_file $tmp_files 2>/dev/null 1>&2
		
		#cleanup (nonoptional here)
		if [ $(( $( du -c $tmp_files | grep total | sed "s/total//g" ) - 1024 )) -le \
		     $( du -c $stitched_file | grep total | sed "s/total//g" ) ]; then
			rm $tmp_files
		else
			echo "Sorry: hadd couldn't join ROOT files properly."
			rm $stitched_file
		fi
	fi
}

#=====================================================================================
#-------------------------------------------------------------------------------------
#da main
#rebuild the positional parametes array and
#pass it to the function that parses them
PARAMS=""
while [ "$1" != "" ]; do
	PARAMS=$PARAMS" "$1
	shift
done
msim_parse_cmd_line $PARAMS

if [ $VERBOSE_FLAG -eq 1 ]; then
	echo "*** Welcome in MULTISIM ***"
	echo "running with "$NB_ONLINE_CPUs" instances of "$PROGRAM
fi

#run the simulation
msim_run_sim

#join the root files (if they sum at less than 99GiB)
if [ $VERBOSE_FLAG -eq 1 ]; then
	echo "Attempting to joing ROOT files..."
fi
msim_join_root_files $( ls $OUTPUT_FILE.proc_?? ) $OUTPUT_FILE

if [ $VERBOSE_FLAG -eq 1 ]; then
	echo "***Done. Goodbye.***"
fi

#the end.
