#!/bin/bash
#SBATCH --partition=cputiny
#SBATCH --exclusive
#SBATCH --time=00:02:00
#SBATCH --ntasks=28
#SBATCH --job-name=testepcdi
#SBATCH --hint=nomultithread
cd $SLURM_SUBMIT_DIR

#### Exporte as variaveis relevantes para o job ########
export OMP_NUM_THREADS=$SLURM_NTASKS
#### Execução do job ####
echo $SLURM_NTASKS
echo $SLURM_JOB_NODELIST

mpirun -np $SLURM_NTASKS ./mapmpi.x
