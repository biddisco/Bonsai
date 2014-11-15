#!/bin/bash

#
# usage
#
echo "Usage : %1:server-port ($1)"
echo "        %2:OpenMP enabled ($2)"
echo "        %3:Root of binary tree ($3)"
echo ""
#
# Get the IP address of whoever called this script via SSH
# we will use this to connect back using pvserver reverse connection
#
ip=`echo $SSH_CLIENT | awk '{print $1}'`
echo "Caller IP Address is : " $ip

if [ $2 -eq 1 ] ; then
  openmpdepth="$[8 / 2]"
#  openmpflags="-d $openmpdepth"
else
  openmpdepth="1"
fi

export OMP_NUM_THREADS=$openmpdepth
export DISPLAY=:1

#aprun -n 64 -N 1 killall renderer_1k_auto
# $ROOT/bin/bonsai_clrshm 1

export MPICH_CPUMASK_DISPLAY=1
export MPICH_MAX_THREAD_SAFETY=multiple

export DYLD_LIBRARY_PATH=/Users/biddisco/build/pv4/lib:/Users/biddisco/apps/boost-1_56_0/lib:/Users/biddisco/apps/hdf5_1_8_cmake/lib:/Users/biddisco/apps/bbp/lib:/Users/biddisco/apps/h5fddsm/lib:$DYLD_LIBRARY_PATH
export PV_PLUGIN_PATH=/Users/biddisco/build/bbp/bin

ROOT=$3

killall bonsai_driver

/usr/local/bin/mpiexec -np 2 $ROOT/bin/bonsai_driver << EOF
  $ROOT/bin/libbonsai_snapserve_mpi.dylib -i $ROOT/fl.1M.txt --noquicksync -l 5
  $ROOT/bin/libbonsai_pvserver.dylib -rc -ch=$ip -sp=$1
