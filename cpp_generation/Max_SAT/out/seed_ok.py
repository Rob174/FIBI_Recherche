from h5py import File
from pathlib import Path
import argparse
import pandas as pd
import numpy as np
if __name__ == "__main__":
    
    parser = argparse.ArgumentParser()
    parser.add_argument('-s', help='seed index in metadata', default=7)
    parser.add_argument('-c', help='num_clust index in metadata', default=3)
    parser.add_argument('-i', help='impr_class index in metadata', default=5)
    parser.add_argument('-n', help='num_points index in metadata', default=0)
    args = parser.parse_args()
    id_seed = args.s
    id_n_clust = args.c
    id_impr_class = args.c
    id_n_points = args.n
    Ldf = []
    with File("dataset.hdf5",'r') as f:
        for d in f['metadata']:
            a = np.array(f['metadata'][d])
            Ldf.append({"NUM_CLUST":a[id_n_clust],"SEED_POINTS": a[id_seed], "IMPR_CLASS": a[id_impr_class], "NUM_POINTS": a[id_n_points]})
    df = pd.DataFrame(Ldf)
    
    for inst in sorted(df['SEED_POINTS'].unique()):
        for n_clust in sorted(df['NUM_CLUST'].unique()):
        
            df_inst = df.query(f"NUM_CLUST == {n_clust} & SEED_POINTS == {inst}")
            try:
                n = df_inst['NUM_POINTS'].unique()[0]
                if n <= n_clust:
                    continue
            except:
                pass
            if len(df_inst) == 2000:
                print(f"OK\t{int(inst):02d}\t\t{int(n_clust):02d}")
            else:
                print(f"NOT OK\t{int(inst):02d}\t\t{int(n_clust):02d} with {len(df_inst)}")