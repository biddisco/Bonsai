#!/bin/bash

#
# usage
#
echo "Usage : %1:Session name ($1)"
echo "        %2:Hours needed ($2)"
echo "        %3:Minutes needed ($3)"
echo "        %4:server-num-nodes ($4)"
echo "        %5:server-num-tasks-per-node ($5)"
echo "        %6:server-binary ($6)"
echo "        %7:server-port ($7)"
echo "        %8:OpenMP enabled ($8)"
echo "        %9:Partition ($9)"
echo "        %10:reservation (${10})"
echo ""
#
# Get the IP address of whoever called this script via SSH
# we will use this to connect back using pvserver reverse connection
#
ip=`echo $SSH_CLIENT | awk '{print $1}'`
echo "Caller IP Address is : " $ip

# -------------------------------------------------------------------
# Function to create job script from params 
# -------------------------------------------------------------------

function write_script
{
# setup vars we will use
JOB_NAME=$1
NSERVERS=$[ $4 * $5 ]

echo "Nservers is "$NSERVERS
PARTITION=""
RESERVATION=""
# create strings for partition and reservation
if [ "${9}" != "" ]
then
 PARTITION='#SBATCH --partition='${9}
fi
if [ "${10}" != "" ]
then
 RESERVATION='#SBATCH --reservation='${10}
fi

if [ $8 -eq 1 ] ; then
  openmpdepth="$[16 / $5]"
#  openmpflags="-d $openmpdepth"
else
  openmpdepth="1"
#  openmpflags=""
fi

# create the job script
echo "Creating job $JOB_NAME"

#====== START of JOB SCRIPT =====
cat << _EOF_ > ./submit-job.bash
#!/bin/bash

#SBATCH --job-name=$JOB_NAME
#SBATCH --time=$2:$3:00
#SBATCH --nodes=$4
#SBATCH --constraint=startx
$PARTITION
$RESERVATION

export DISPLAY=:0.0

module load boost/1.56.0
module load cudatoolkit
#module load viz

# boost
export LD_LIBRARY_PATH=/apps/daint/boost/1.56.0/gnu_482/lib:\$LD_LIBRARY_PATH
# hdf5
export LD_LIBRARY_PATH=/users/biddisco/apps/daint/hdf5_1_8_cmake/lib:\$LD_LIBRARY_PATH
# nvidia GL
export LD_LIBRARY_PATH=/opt/cray/nvidia/default/lib64:\$LD_LIBRARY_PATH
# paraview
export LD_LIBRARY_PATH=/scratch/daint/biddisco/build/pv4/lib:\$LD_LIBRARY_PATH
# plugins 
export LD_LIBRARY_PATH=/scratch/daint/biddisco/build/bbp-pv/bin:\$LD_LIBRARY_PATH
export PV_PLUGIN_PATH=/scratch/daint/biddisco/build/bbp-pv/bin

export OMP_NUM_THREADS=$openmpdepth
#export CRAY_CUDA_MPS=1
#export CRAY_CUDA_PROXY=1

aprun -n $NSERVERS -N $5 $openmpflags $6 --disable-xdisplay-test -rc -ch=$ip -sp=$7

_EOF_

#====== END of JOB SCRIPT =====

chmod 775 ./submit-job.bash

}

# -------------------------------------------------------------------
# End function
# -------------------------------------------------------------------

write_script $1 $2 $3 $4 $5 $6 $7 $8 $9 ${10}

echo "Job script written"
cat ./submit-job.bash


#
# submit the job
#
/apps/daint/slurm/default/bin/sbatch ./submit-job.bash

#
# wipe the temp file
#
rm ./submit-job.bash
