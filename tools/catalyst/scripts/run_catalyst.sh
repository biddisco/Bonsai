#-! /- bin/sh
export OMP_NUM_THREADS=6
export DISPLAY=:1

#aprun -n 64 -N 1 killall renderer_1k_auto
#aprun -n 64 -N 1 ./bonsai_clrshm 64

export MPICH_CPUMASK_DISPLAY=1
export MPICH_MAX_THREAD_SAFETY=multiple

export DYLD_LIBRARY_PATH=/Users/biddisco/build/pv4/lib:~/apps/boost-1_56_0/lib:~/apps/hdf5_1_8_cmake/lib/:~/apps/bbp/lib/:~/apps/h5fddsm/lib/:$DYLD_LIBRARY_PATH
export PV_PLUGIN_PATH=/Users/biddisco/build/bbp/bin

ROOT=/Users/biddisco/build/bcatalyst

mpiexec -np 2 $ROOT/bin/bonsai_driver << EOF
  $ROOT/bin/libbonsai_snapserve_mpi.dylib -i $ROOT/fl.1M.txt --noquicksync -l 25
  $ROOT/bin/libbonsai_catalyst_mpi.dylib -I --noquicksync 
