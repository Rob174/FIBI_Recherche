#!/bin/bash
#SBATCH --job-name=myjob0
#SBATCH --output=myjob0.out
#SBATCH --error=myjob0.err
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=90
#SBATCH --time=24:00:00
/mnt/c/Users/robin/Documents/Cours/Poly/Recherche/FIBI_Recherche/cpp_generation/FIBI/FIBI -path_out /mnt/c/Users/robin/Documents/Cours/Poly/Recherche/FIBI_Recherche/cpp_generation/FIBI/dataset1.txt -root_data /mnt/c/Users/robin/Documents/Cours/Poly/Recherche/FIBI_Recherche/cpp_generation/FIBI/data/folder/ -dataset 1 -n_treads 10 -all true -seed_start 0 -seed_end -1 -problem maxsat -num_rep 1000
