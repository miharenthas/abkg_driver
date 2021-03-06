#! /bin/bash

#this script drives the generation of ABKG's outputs according to
#a range of energies. It proceeds as follows
# 1) parse the command line (see below for option list)
# 2) generate the needed files from a template
# 3) launch ABKG in batch on the generated files
#    redirecting the stdout to a log file
# 4) converts the generated output files from .hst to .root
# 5) generate the ASCII events (compressed)
# 6) cleans up

#NOTE: this script requires GNU coreutils and other stuff like "pgrep"
#      an automated test will be available soon. Until then, check
#      that you have the needed programs.

#global constants

AD_HOME_DIR="./"
ROOT_IO_DIR="../abkg_working"
CONFIG_FILE_DIR=$ROOT_IO_DIR"/cnf_files" #config file directory
OUTPUT_FILE_DIR=$ROOT_IO_DIR"/output_files" #output file directory
#NOTE: ABKG has strong limitation on the length of the file name
#      thus relative paths are used. Be careful!

TEMPLATE_ROOT="132sn_208pb_"
TEMPLATE_FILE=$TEMPLATE_ROOT"TEMPLATE.abkg" #template file full name
LOG_FILE="ABKG_runner.log"
NB_ONLINE_CPUs=$(nproc) #number of available CPUs
NB_JOBS_GLOBAL=0 #number of jobs requested
SBKG_MEM_LIMIT=0 #how much memory is a program allowed to use

#global variables

nb_events="" #number of events that are going to be generated by gbkg
filename="" #name of the output file
energy_range="" #range of energies to be investigated
en_gamma=""
en_sigma=""
ang_cut=""
spc_start="" #where the spectrum starts
spc_stop="" #where the spectrum stops
spc_bin="" #bin of the spectrum
ang_start="" #start of the considered solid angle
ang_stop="" #end of the solid angle
ang_step="" #stepping of the angle (NOTE: 361 seems to be the max)
detector_list="" #list of detectors to simulate
target_thk="" #target thickness
atomic_xs="" #atomic processes cross section

#flags

do_simulation=0 #also run a simulation
mux_simulation=0 #multiplex the simulation
cleanup=1 #a flag that controls whether intermediate files are left alone
no_compresstion=0 #a flag for compression of output.
save_geometry=0 #save the geometry in the simulation output (for event display)

#=====================================================================================
#parsing and evironment setup:
#function "ad_check_env()" checks the environment and sets it up
#function "ad_parse_input()" parses the options and sets the variables

#-------------------------------------------------------------------------------------
#check that the environment is properly set
ad_check_env(){
	#home directory
	if [ -z $ABKG_HOME_DIR ]; then
		echo "ABKG_HOME_DIR not set. Please do it."
		exit 1
	fi

	#check that the executables are in the path
	if [ -z "$(echo $PATH | grep \"abkg\")" ]; then
		if [ -x $ABKG_HOME_DIR/src/abkg ]; then
			export PATH=$PATH:$ABKG_HOME_DIR/src/
		else
			echo "abkg is unreachable. Did you compile it?"
			exit 1
		fi
	fi
	if [ -z "$(echo $PATH | grep \"gbkg\")" ]; then
		if [ -x $ABKG_HOME_DIR/hst2ascii/gbkg ]; then
			export PATH=$PATH:$ABKG_HOME_DIR/hst2ascii/
		else
			echo "gbkg is unreachable. Did you compile it?"
			exit 1
		fi
	fi
	if [ -z "$(echo $PATH | grep \"sbkg\")" ] && [ $(( $do_simulation + $mux_simulation )) -ne 0 ]; then
		echo PIP
		if [ -x $ABKG_HOME_DIR/simulator/sbkg ]; then
			export PATH=$PATH:$ABKG_HOME_DIR/simulator/
		else
			echo "sbkg is unreachable. Did you compile it?"
			exit 1
		fi
	fi
	
	#make the script aware of itself
	AD_HOME_DIR=$ABKG_HOME_DIR/shell_driver
	
	#set the memory limit
	SBKG_MEM_LIMIT=$( echo $( cat /proc/meminfo | grep MemFree ) | sed 's/MemFree: //g;s/ kB//g' )
	SBKG_MEM_LIMIT=$(( $SBKG_MEM_LIMIT*1024*3/4 )) #in bytes, and allow only 3/4 to be used
	
}

#-------------------------------------------------------------------------------------
#parse the input
ad_parse_input(){
	while [ $# -gt "0" ]; do
		case $1 in
			--template-root )
				shift
				TEMPLATE_ROOT=$1
				TEMPLATE_FILE=$TEMPLATE_ROOT"TEMPLATE.abkg"
				;;
			-n | --nb-events )
				shift
				nb_events=$1
				;;
			-f | --file-name )
				shift
				filename=$1
				;;
			-e | --energy )
				shift;
				energy_range=$1
				;;
			-E | --energy-range )
				shift
				begin=$1; shift
				step=$1; shift
				end=$1
				energy_range=$(seq -s " " $begin $step $end)
				;;
			--en-gamma )
				shift
				en_gamma=$1
				;;
			--en-sigma )
				shift
				en_sigma=$1
				;;
			--ang_cut )
				shift
				ang_cut=$1
				;;
			--target-thickness )
				shift
				target_thk=$1
				;;
			-x | --atomic-xsection )
				shift
				atomic_xs=$1
				;;
			-X | --atomic-xs-range )
				shift
				begin=$1; shift;
				step=$1; shift;
				end=$1;
				atomic_xs=$(seq -s " " $begin $step $end);
				;;
			--spc-start )
				shift
				spc_start=$1
				;;
			--spc-stop )
				shift
				spc_stop=$1
				;;
			-b | --spc-bin )
				shift
				spc_bin=$1
				;;
			-a | --angle-start )
				shift
				ang_start=$1
				;;
			-A | --angle-stop )
				shift
				ang_stop=$1
				;;
			-t | --angle-step )
				shift
				ang_step=$1
				;;
			--detector-list )
				shift
				detector_list=$1
				;;
			--no-compression )
				shift
				no_compression=1
				;;
			--keep-everything )
				shift
				cleanup=0
				;;
			--do-simulation )
				shift
				do_simulation=1
				;;
			--mux-simulation )
				shift
				do_simulation=1 #of course...
				mux_simulation=1
				;;
			--max-jobs )
				shift
				NB_ONLINE_CPUs=$1
				;;
			--save-geometry )
				shift
				save_geometry=1
				;;
			-h | --help )
				cat "$AD_HOME_DIR/doc/abkg_driver_help"
				exit 0
				;;
			--output-dir )
				shift
				ROOT_IO_DIR=$1"/"
				
				ad_check_a_dir $ROOT_IO_DIR
				
				CONFIG_FILE_DIR=$ROOT_IO_DIR"/cnf_files"
				OUTPUT_FILE_DIR=$ROOT_IO_DIR"/output_files"
				;;
			* )
				shift
				;;
		esac
	done
	#now, some defaults in case they haven't been specified
	if [ -z $nb_events ]; then nb_events=100000; fi
	if [ -z $filename ]; then filename=".hst"; fi
	if [ -z "$energy_range" ]; then energy_range=500; fi
	if [ -z $en_gamma ]; then en_gamma="1000."; fi
	if [ -z $en_sigma ]; then en_sigma="100."; fi
	if [ -z $ang_cut ]; then ang_cut="50."; fi
	if [ -z $spc_start ]; then spc_start="30"; fi
	if [ -z $spc_stop ]; then spc_stop="3030"; fi
	if [ -z $spc_bin ]; then spc_bin=1001; fi
	if [ -z $ang_start ]; then ang_start="0.0"; fi
	if [ -z $ang_stop ]; then ang_stop="180.0"; fi
	if [ -z $ang_step ]; then ang_step=361; fi
	if [ -z $no_compression ]; then no_compression=0; fi
	if [ -z $target_thk ]; then target_thk="1.134"; fi
	if [ -z "$atomic_xs" ]; then atomic_xs="1.07e5"; fi
	if [ -z $detector_list ]; then
		detector_list="TARGETATMOSPHERE:AtmoVacuum" #TARGET:LeadTarget removed
		detector_list=$detector_list":TARGETWHEEL:TARGETSHIELDING:CRYSTALBALL:"
	fi

	#count the jobs
	for nrg in $energy_range; do
		for xs in $atomic_xs; do
			NB_JOBS_GLOBAL=$(( $NB_JOBS_GLOBAL+1 ))
		done
	done
	echo "Total jobs requested: $NB_JOBS_GLOBAL"
}

#=====================================================================================
#checkingutilities:
#function "ad_check_a_dir()" checks whether a directory is empty and asks for action
#function "ad_wait_for_jobs()" waits for processes of a certain name to terminate
#function "ad_job_control()" keeps in check jobs that may fail, ROOT style (by invoking gdb
#         and basically stalling the system).
#function "ad_count_global_jobs()" keeps the job count up to date

#-------------------------------------------------------------------------------------
#this function checks whether a directory ($1) is exists and it's empty
ad_check_a_dir(){
	#check if the directory already exists
	#if so, empty it (with warning); else create it.
	if [ -d "$1" ]; then
		if [ -n "$(ls -A $1)" ]; then
			echo -n "WARNING: $1 is not empty. Proceed and remove its content?(Y/N) "
			
			#ask for confirmation before removing everything
			proceed=""
			while [ -z $proceed ]; do
				read proceed
				case $proceed in
					y | Y | yes | Yes | YES )
						rm -r $1/*
						;;
					n | N | no | No | NO )
						exit 1
						;;
					* )
						echo -n "$proceed is an invalid answer. Proceed and empty?(Y/N) "
						proceed=""
						;;
				esac
			done
		fi
	else
		mkdir $1
	fi
}

#-------------------------------------------------------------------------------------
#wait for unfinished processes of a certain name
ad_wait_for_jobs(){
	#at the end of things, wait for all the jobst to terminate
	echo "Waiting for all the jobs to terminate... "
	t_pids=$( pgrep -P $$  "$1" )
	wait $t_pids
	echo "All completed NOW: $(date +%T)."
}

#-------------------------------------------------------------------------------------
#job control
#NOTE: a variable "nb_active_jobs" must be defined by the caller!
ad_job_control(){
	nb_active_jobs=$( pgrep -P $$ -c "$1" )
	while [ $nb_active_jobs -ge $NB_ONLINE_CPUs ]; do
		sleep 1
		nb_active_jobs=$( pgrep -P $$ -c "$1" )
		
		#because, when Geant3 crashes, we get stuck on gdb for ages
		#here is a check we will also perform. It's cumbersome
		#because of the way ROOT launches gdb on crash.
		nb_failed=$( pgrep -c "gdb" )
		if [ $nb_failed -gt 0 ]; then
			for a_job in $( pgrep -P $$ -c "$1" -d " " ); do
				if [ $( pgrep -P $a_job -c "sh" ) -gt 0 ]; then
					kill -SIGKILL $( pgrep -P $( pgrep -P $( pgrep -P $a_job "sh" ) "gdb-backtrace" ) "gdb" )
					
					echo "WARNING: root process $a_job crashed and was killed. See log for details."
					echo "         You may have to run this particular job again..."
				fi
			done
		fi
	done
}

#-------------------------------------------------------------------------------------
#checks the number of global jobs
ad_count_global_jobs(){
	global_job_count=""
	if [ "$1" == "ad_piped_sim" ]; then
		gloabl_job_count=0
		for xs in $atomic_xs; do
			global_job_count=$(( $job_count + $# -1 ))
		done
	else
		global_job_count=$#
	fi
	
	if [ $NB_JOBS_GLOBAL -ne $global_job_count ]; then
		echo "Number of jobs update: were $NB_JOBS_GLOBAL, now are $global_job_count."
		NB_JOBS_GLOBAL=$global_job_count
	fi
}

#=====================================================================================
#workhorses:
#function "ad_make_config_files()" parses the template according to specification to
#         create a bunch of ABKG config files.
#function "ad_join_root_files()" stitches root files together using hadd. It takes in
#         many names and uses the last one as the output.
#function "ad_ABKG_runner()" runs ABKG on the generated config files
#function "ad_convert_histogram()" converts ABKG histograms to root files
#function "ad_generate_events()" generates ASCII event files to be used with the
#         R3BAsciiGenerator.
#functions "ad_run_simuation_[F,P,MF,MP]()" run the simulation using the R3B simulator
#          on in various ways.

#-------------------------------------------------------------------------------------
#this function assembles the configuration files
#iterating on the energies
#NOTE: abkg has very limited file name lenght support. In order to not
#      run out, abkg will be launche in $ROOT_IO_DIR/cnf_files, making
#      paths relative to that (which guarantee brievity)
ad_make_config_files(){
	ad_check_a_dir $CONFIG_FILE_DIR
	
	#do the copying and the editing
	current_file=""
	for nrg in $energy_range; do
		current_file="$CONFIG_FILE_DIR/$TEMPLATE_ROOT$nrg-$spc_start-$spc_stop.abkg"
		cp $AD_HOME_DIR/$TEMPLATE_FILE $current_file
	
		#build the complete filename, escaping the slashes properly
		complete_filename="../output_files/amev"$(printf "%04d" $nrg)"_kev"$spc_start"-"$spc_stop$filename 
		complete_filename=$( echo $complete_filename | sed 's/\//\\\//g' )
	
		sed -i "s/##OUTPUT_NAME/$complete_filename/g" $current_file
		sed -i "s/##EN_GAMMA/$en_gamma/g; s/##EN_SIGMA/$en_sigma/g; s/##ANG_CUT/$ang_cut/g" \
			  $current_file
		sed -i "s/##BEAM_ENERGY/$nrg/g" $current_file
		sed -i "s/##SPC_START/$spc_start/g; s/##SPC_STOP/$spc_stop/g; s/##SPC_BIN/$spc_bin/g" \
			  $current_file
		sed -i "s/##ANG_START/$ang_start/g; s/##ANG_STOP/$ang_stop/g; s/##ANG_STEP/$ang_step/g" \
			  $current_file
	done
}

#-------------------------------------------------------------------------------------
#this function stitches root files together
#the last input is the output file name
ad_join_root_files(){
	tmp_files=""
	while [ "$2" != "" ]; do
		tmp_files=$tmp_files" "$1
		shift;
	done
	stitched_file=$1
	
	if [ $( du -c $tmp_files | grep total | sed "s/total//g" ) -le 103809024 ]; then #if the total expected
	                                                                                 #size is less than 99GiB
	                                                                                 #join.
	
		hadd -f $stitched_file $tmp_files 1>$LOG_FILE 2>&1
		
		#cleanup (nonoptional here)
		if [ $(( $( du -c $tmp_files | grep total | sed "s/total//g" ) - $(( 1024*$NB_ONLINE_CPUs )) )) -le \
		     $( du -c $stitched_file | grep total | sed "s/total//g" ) ]; then
			rm $tmp_files
		else
			echo "Sorry: hadd couldn't join ROOT files properly."
			rm $stitched_file
		fi
	else
		echo "WARNING: too much data to join root files. Skipping."
	fi
}

#-------------------------------------------------------------------------------------
#this function launces $NB_ONLINE_CPUs ABKG processes
#on the files generated by "ad_make_config_files"
ad_ABKG_runner(){
	#check that the output file directory
	ad_check_a_dir $OUTPUT_FILE_DIR
	
	#create an empty log file
	echo "" > $LOG_FILE
	
	#go into the config file dir, where ABKG lives
	cd $CONFIG_FILE_DIR
	
	#get the config files
	files_to_generate=$( ls *.abkg )
	
	#count the jobs
	ad_count_global_jobs $files_to_generate
	
	#main loop on the enrgies
	for a_file in $files_to_generate; do
		#launch abkg (in a hacky way, but it works)
		#and put it into background
		echo $a_file | abkg >> $OLDPWD/$LOG_FILE &
	
		#make a "wait" queue. Get the number of active abkgs
		#on the shell, while this number is equal (or more)
		#than the online CPUs, wait and look again. As soon as
		#one slot is free, exit the wait loop.
		echo "RUN: abkg launched on $a_file."
		
		#job control
		ad_job_control "abkg"
	done
	
	#go back where you were
	cd $OLDPWD
	
	#wait for all the jobs to finish
	ad_wait_for_jobs "abkg"
}

#-------------------------------------------------------------------------------------
#this function runs the conversion of ABKG's output files into ROOT files
ad_convert_histograms(){
	files_to_convert=$( ls $OUTPUT_FILE_DIR/*.hst )
	
	ad_count_global_jobs $files_to_convert
	
	for a_file in $files_to_convert; do
		#launch one instance of h2root
		#redirecting its output to the logfile
		h2root $a_file >> $LOG_FILE &
		
		#again, make a wait queue based on the number of jobs active
		echo "CONV: conversion of $a_file launched."
		
		#job control
		ad_job_control "abkg"
	done
	
	#at the end of things, wait for all the jobst to terminate
	ad_wait_for_jobs "h2root"
	
	#cleanup the histograms (no longer useful at this point)
	if [ $cleanup -eq 1 ]; then
		rm $files_to_convert
	fi
}

#-------------------------------------------------------------------------------------
#last but not least, this function converts Dr. Dominic Rossi's script
ad_generate_events(){
	files_to_process=$( ls $OUTPUT_FILE_DIR/*.root )
	
	ad_count_global_jobs $files_to_process
	
	for a_file in $files_to_process; do
		for xs in $atomic_xs; do
			#launch one instance of "gbkg"
			#NOTE: for different targets, different options have to be
			#      passed to gbkg (which does not really use a config file, yet)
			#      this means some more nouance is needed here.
			#      Also, when a fullblown program for the simulations will be
			#      available, this will live in a pipe afrer gbkg.
			
			out_fname=""
			if [ $no_compression -eq 1 ]; then
				out_fname=$( echo $a_file | sed "s/.root/_"$xs"mBarn.dat/g" )
				gbkg $a_file -o $out_fname --nb-events $nb_events \
					           --target-thickness $target_thk \
					           --tot-cs $xs \
					           --no-compression & #this should be silent
			else
				out_fname=$( echo $a_file | sed "s/.root/_"$xs"mBarn.bz2/g" )
				gbkg $a_file -o $out_fname --nb-events $nb_events \
					           --tot-cs $xs \
					           --target-thickness $target_thk &
			fi
		
			echo "GEN: generation of $out_fname launched."
		done
		
		#job control
		ad_job_control "gbkg"
	done
	
	#at the end of things, wait for all the jobst to terminate
	ad_wait_for_jobs "gbkg"
	
	#cleanup the histograms (no longer useful at this point)
	if [ $cleanup -eq 1 ]; then
		rm $files_to_process
	fi
}

#-------------------------------------------------------------------------------------
#drive the simulator, in four ways:
#1) from file
#2) from pipe
#3) multiplexed from file
#4) multiplexed from pipe

#1) from file
ad_run_simulation_F(){
	#gather the ASCII source files
	if [ $no_compression -eq 1 ]; then
		files_to_process=$( ls $OUTPUT_FILE_DIR/*.dat )
	else
		files_to_process=$( ls $OUTPUT_FILE_DIR/*.bz2 )
	fi
	
	ad_count_global_jobs $files_to_process
	
	#loop on them
	for a_file in $files_to_process; do
		if [ $no_compression -eq 1 ]; then
			output_name=$( echo $a_file | sed "s/\.dat/_sbkg\.root/g" ) #make the output name
			par_name=$( echo $a_file | sed "s/\.dat/_par_sbkg\.root/g" ) #make the parfile name
		else
			output_name=$( echo $a_file | sed "s/\.bz2/_sbkg\.root/g" ) #make the output name
			par_name=$( echo $a_file | sed "s/\.bz2/_par_sbkg\.root/g" ) #make the parfile name
		fi
		
		#launch the program on the ascii file, suppressing the output
		if [ $no_compression -eq 1 ]; then
			if [ $save_geometry -eq 1 ]; then
				sbkg $a_file --nb-events $nb_events \
				             --detector-list $detector_list \
				             -o $output_name \
				             -p $par_name \
				             --no-magnet \
				             --mem-limit $SBKG_MEM_LIMIT \
				             --save-geometry \
				             1>>$LOG_FILE 2>&1 &
			else
				sbkg $a_file --nb-events $nb_events \
				             --detector-list $detector_list \
				             -o $output_name \
				             -p $par_name \
				             --no-magnet \
				             --mem-limit $SBKG_MEM_LIMIT \
				             1>>$LOG_FILE 2>&1 &
			fi
		else
			if [ $save_geometry -eq 1 ]; then
				bzcat $a_file | sbkg --nb-events $nb_events \
				                     --detector-list $detector_list \
				                     -o $output_name \
				                     -p $par_name \
				                     --no-magnet \
				                     --mem-limit $SBKG_MEM_LIMIT \
				                     --save-geometry \
				                     1>>$LOG_FILE 2>&1 &
			else
				bzcat $a_file | sbkg --nb-events $nb_events \
				                     --detector-list $detector_list \
				                     -o $output_name \
				                     -p $par_name \
				                     --no-magnet \
				                     --mem-limit $SBKG_MEM_LIMIT \
				                     1>>$LOG_FILE 2>&1 &
			fi 
		fi
		
		echo "SIM: simulation of $a_file launched."
		
		#job control
		ad_job_control "sbkg"
	done
	
	#at the end of things, wait for all the jobst to terminate
	ad_wait_for_jobs "sbkg"
	
	#clean up the sources, if requested
	if [ $cleanup -eq 1 ]; then
		rm $files_to_process
	fi
}

#2) from pipe
#note that this runs the generation AND the simulation at once
#and that's kinda the point.
ad_run_simulation_P(){
	#gather the ASCII source files
	#NOTE: this assumes that the files bear a .dat extension
	#      which is kinda big. A more general version may follow.
	files_to_process=$( ls $OUTPUT_FILE_DIR/*.root )
	
	ad_count_global_jobs "ad_piped_sim" $files_to_process
	
	for a_file in $files_to_process; do
		for xs in $atomic_xs; do
			output_name=$( echo $a_file | sed "s/\.root/_"$xs"mBarn_sbkg\.root/g" ) #make the output name
# 			par_name=$( echo $a_file | sed "s/\.root/_"$xs"mBarn_par_sbkg\.root/g" ) #make the parfile name
			par_name=/dev/null #removed because of ROOT bugs
		
			#launch the program on the ascii file, suppressing the output
			if [ $save_geometry -eq 1 ]; then
				gbkg $a_file --nb-events $nb_events \
					           --tot-cs $xs \
					           --target-thickness $target_thk | \
				sbkg --nb-events $nb_events \
					   --detector-list $detector_list \
					   -o $output_name -p $par_name \
					   --no-magnet --save-geometry \
					   --mem-limit $(( $SBKG_MEM_LIMIT/$NB_ONLINE_CPUs )) \
					   1>>$LOG_FILE 2>&1 &
			else
				gbkg $a_file --nb-events $nb_events \
					           --tot-cs $xs \
					           --target-thickness $target_thk | \
				sbkg --nb-events $nb_events \
					   --detector-list $detector_list \
					   -o $output_name -p $par_name \
					   --no-magnet --save-geometry \
					   --mem-limit $(( $SBKG_MEM_LIMIT/$NB_ONLINE_CPUs )) \
					   1>>$LOG_FILE 2>&1 &
			fi
		
			echo "SIM: simulation pipeline of $a_file launched."
		
			#job control
			ad_job_control "sbkg"
		done
	done
	
	#at the end of things, wait for all the jobst to terminate
	ad_wait_for_jobs "sbkg"
	
	#clean up the sources, if requested
	if [ $cleanup -eq 1 ]; then
		rm $files_to_process
	fi
}

#3) TODO: coming soon, because a tool program is needed.
#...

#4) multiplex on pipe(s)
ad_run_simulation_MP(){
	#gather the ASCII source files
	#NOTE: this assumes that the files bear a .dat extension
	#      which is kinda big. A more general version may follow.
	files_to_process=$( ls $OUTPUT_FILE_DIR/*.root )
	
	ad_count_global_jobs "ad_piped_sim" $files_to_process
	
	if [ $NB_JOBS_GLOBAL -ge $NB_ONLINE_CPUs ]; then
		echo "NOTE: using --mux-simulation for more jobs than CPUs is dumb."
		echo "      Switching to normal pipeline."
		ad_run_simulation_P
		return
	fi
	
	nb_evt_per_job=$((nb_events/$NB_ONLINE_CPUs + 1))
	
	#loop on each file splitting the job in how many CPUs we have
	#TODO: use temporary fikes for this
	for a_file in $files_to_process; do
		for xs in $atomic_xs; do
			echo "MSIM: launching simulation pipeline of $a_file"
			echo "      for xsection $xs mBarn:"
			for a_job in $( seq -s " " $NB_ONLINE_CPUs ); do
				output_name=$( echo $a_file | sed "s/\.root/_"$xs"mBarn_sbkg_$a_job\.root/g" ) #output name
				#par_name=$( echo $a_file | sed "s/\.root/_"$xs"mBarn_par_sbkg_$a_job\.root/g" ) #parfile name
				par_name=/dev/null #removed because of ROOT bugs
			
				gbkg $a_file --nb-events $nb_evt_per_job \
					           --tot-cs $xs \
					           --target-thickness $target_thk | \
				sbkg --nb-events $nb_evt_per_job \
					   --detector-list $detector_list \
					   -o $output_name -p $par_name \
					   --no-magnet --save-geometry \
					   --mem-limit $(( $SBKG_MEM_LIMIT/$NB_ONLINE_CPUs )) \
					   1>>$LOG_FILE 2>&1 &
			
				echo -e "\tJob $a_job has started."
			done
		
			#wait for all the jobs to complete, policing gdb calls
			#because, when Geant3 crashes, we get stuck on gdb for ages
			#here is a check we will also perform. It's cumbersome
			#because of the way ROOT launches gdb on crash.
			while [ $( pgrep -P $$ -c "sbkg" ) -gt 0 ]; do
				sleep 1
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
		
			#join the files together:
			#again, parfiles are excluded because they are bugged
			#ad_join_root_files $( ls $OUTPUT_FILE_DIR/*"$xs"mBarn_par_sbkg_[1-9]*.root ) \
			#                   $( echo $a_file | sed "s/\.root/_"$xs"mBarn_par_sbkg\.root/g" )
			
			ad_join_root_files $( ls $OUTPUT_FILE_DIR/*"$xs"mBarn_sbkg_[1-9]*.root ) \
			                   $( echo $a_file | sed "s/\.root/_"$xs"mBarn_sbkg\.root/g" )
		done
	done
	
	#clean up the sources, if requested
	if [ $cleanup -eq 1 ]; then
		rm $files_to_process
	fi
}

#=====================================================================================
#-------------------------------------------------------------------------------------
#main program (the driver itself)

echo "***Welcome in the ABKG shell driver***"

#rebuild the positional parametes array and
#pass it to the function that parses them
PARAMS=""
while [ "$1" != "" ]; do
	PARAMS=$PARAMS" "$1
	shift
done
ad_parse_input $PARAMS

#check the environment
ad_check_env

echo "Settings:"
echo -e "\tOutput file name tail: $filename"
echo -e "\tMemory cap: $(( SBKG_MEM_LIMIT/(1024**2) )) MiB"
echo -e "\tEnergy range: $energy_range"
echo -e "\tEN_GAMMA: $en_gamma"
echo -e "\tEN_SIGMA: $en_sigma"
echo -e "\tANG_CUT: $ang_cut"
echo -e "\tTarget thickness: $target_thk"
echo -e "\tAtomic cross section: $atomic_xs"
echo -e "\tSpectrum from $spc_start to $spc_stop"
echo -e "\tSpectrum's bin: $spc_bin"
echo -e "\tSolid angle from $ang_start to $ang_stop"
echo -e "\tAngle step: $ang_step"
echo -e "\tEvents: $nb_events"
if [ $do_simulation -eq 1 ]; then
	echo -e "\tDetector list: $detector_list"
fi

#look at the clock at the beginning of ops
wallclock_start=$( date +%F\ %T )

#create the configuration files
ad_make_config_files

#recursively launch ABKG!
ad_ABKG_runner

#convert the output files with h2root
#check whether the program is reachable
thisroot=""
if [ -z $( which "h2root" ) ]; then
	echo "OOPS: h2root is not reachable. Path to \"thisroot.sh\"?"
fi
while [ -z $( which "h2root" ) ]; do
	echo -n "> "; read thisroot
	if [ -f $thisroot ]; then
		source $thisroot || echo "$thisroot is not a valid shell script."
	else
		echo "$thisroot is not reachable."
	fi
done

#now that we have h2root, launch the conversion
ad_convert_histograms

#and generate the output ASCII and/or run the simulation
if [ $do_simulation -eq 0 -o $cleanup -eq 0 ]; then
	ad_generate_events
else
	if [ $mux_simulation -eq 1 ]; then
		ad_run_simulation_MP
	else
		ad_run_simulation_P
	fi
fi
if [ $do_simulation -eq 1 -a $cleanup -eq 0 ]; then
	if [ $mux_simulation -eq 1 ]; then
		echo "DANG: no simulation multiplexing is available yet..."
		exit 1
	else
		ad_run_simulation_F
	fi
fi

#move the gphysi.dat thing into the working folder
if [ -e gphysi.dat ]; then
	mv gphysi.dat $ROOT_IO_DIR
fi

#move the logfile into the io directory
mv $LOG_FILE $ROOT_IO_DIR/$LOG_FILE

#look at the clock now
wallclock_stop=$( date +%F\ %T )

echo "Wall clock at the beginning of operations: --- $wallclock_start"
echo "Wall clock at the end of operations: --------- $wallclock_stop"
echo -n "UNIX epoch delta: ---------------------------- "
echo -n $(($(date -d "$wallclock_stop" +%s)-$(date -d "$wallclock_start" +%s)))
echo " s"

echo "***Done. Goodbye***"
