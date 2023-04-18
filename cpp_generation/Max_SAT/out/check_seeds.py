import argparse
from h5py import File
import numpy as np
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-s", type=int, default=0)
    parser.add_argument("-e", type=int, default=0, dest="e")
    args = parser.parse_args()
    seeds = set()
    with File("dataset.hdf5",'r') as f:
        for k,v in f['metadata'].items():
            a = np.array(v)
            seeds.add(int(v[6]))
    # check missing continuous interval of seeds missing from seeds
    missing_seeds = []
    last_seen = 0
    for i in range(args.s, args.e+1):
        if i not in seeds:
            missing_seeds.append(i)
        else:
            last_seen = max(i,last_seen)
    print("Last seed observed {} and total seeds {}".format(last_seen, len(seeds)))
    print(seeds)
    # print missing seeds by interval [start, end] of continuous missing seeds
    start = 0
    end = 0
    for i in range(len(missing_seeds)):
        if i == 0:
            start = missing_seeds[i]
            end = missing_seeds[i]
        elif missing_seeds[i] == missing_seeds[i-1] + 1:
            end = missing_seeds[i]
        else:
            print("Missing seeds: [{}, {}]".format(start, end))
            start = missing_seeds[i]
            end = missing_seeds[i]