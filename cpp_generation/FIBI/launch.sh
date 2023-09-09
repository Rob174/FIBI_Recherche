#!/bin/bash
#SBATCH --job-name=myjob0
#SBATCH --output=myjob0.out
#SBATCH --error=myjob0.err
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=90
#SBATCH --time=24:00:00
./FIBI/FIBI -path_out /mnt/c/Users/rmoine/FIBI/dataset.txt -root_data /mnt/c/Users/rmoine/FIBI/data/folder/ -dataset 0 -n_treads 16 -impr 0 -opt 3 -seed_start 0 -seed_end -1 -problem tsp -num_rep 1000 > out.log 2 > &1 &

