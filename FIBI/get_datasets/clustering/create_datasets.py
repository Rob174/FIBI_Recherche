"""In charge of creating franti et al datasets and aloise et al benchmark for the clustering problem"""
import numpy as np
from pathlib import Path # type: ignore
import re # type: ignore
from h5py import File
import json

def main_franti():
    """
    Parse the data from the txt files (manually downloaded from http://cs.uef.fi/sipu/datasets/) and create the hdf5 file containing the datasets (points coordinates, number of dimensions and number of clusters for each instance). 
    
    Note: Due to computational constraints, only the instances of A, S, DIM, Unbalanced have been used. This filtering has been done manually with the global_seed of the c++ program.
    
    This script is using the following files:
    
    - data/src_datasets/clustering/benchmark_franti/**/*.txt
    - data/src_datasets/clustering/benchmark_num_clusters.json
        
    And creating the following files:
    
    - data/algorithms_in/clustering/benchmark_clustering.hdf5
    - data/algorithms_in/clustering/benchmark_clustering_mapping.json
    """
    path_data = Path(".") / "data" / "src_datasets" / "clustering" / "benchmark_franti"
    path_num_clusters = Path(".") / "data" / "src_datasets" / "clustering" / "benchmark_num_clusters.json"
    path_out_hdf5 = Path(".") / "data" / "algorithms_in" / "clustering" / "benchmark_clustering.hdf5"
    path_mapping_names = Path(".") / "data" / "algorithms_in" / "clustering" / "benchmark_clustering_mapping.json"
    
    datasets = {}
    for f in path_data.rglob("**/*.txt"):
        if not f.is_file():
            continue
        if f.parent.name not in ["A", "Birch", "DIM", "G2", "S", 'Unbalanced']:
            continue
        matched = False
        for s in ['-ga','-sub','-random','BirchGT','-gt','G2.txt','-cb']:
            if re.search(s,f.name):
                matched = True
        if matched:
            continue
        datasets[f.stem] = {"path":f}
    with open(path_num_clusters) as f:
        mapping_num_clust = json.load(f)
    with File(path_out_hdf5,"w") as cache:
        gpe_pts = cache.create_group('points')
        gpe_dims = cache.create_group('dims')
        gpe_clusters = cache.create_group('num_clusters')
        for i,f in enumerate(datasets):
            with open(datasets[f]['path']) as fp:
                data = [[float(a) for a in re.split(" +",x.strip())] for x in fp.read().strip().split('\n')]
            datasets[f]['points'] = np.array(data,dtype=np.float32)
            datasets[f]['NUM_DIM'] = np.array(datasets[f]['points'].shape)
            gpe_pts.create_dataset(name=str(i),dtype="f",data=datasets[f]['points'].flatten())
            gpe_dims.create_dataset(name=str(i),dtype="f",data=datasets[f]['NUM_DIM'].flatten())
            gpe_clusters.create_dataset(name=str(i),dtype="f",data=np.array([mapping_num_clust[f],mapping_num_clust[f]]))
    with open(path_mapping_names,'w') as f:
        json.dump({i:str(k) for i,k in enumerate(datasets.keys())},f)


def main_aloise():
    """
    Function that parses the aloise et al benchmark for the clustering problem and creates the hdf5 file containing the dataset with the points coordinates and the metadata (number of points and number of dimensions).
    
    ## Files created
    
    - data/algorithms_in/clustering/benchmark_aloise/benchmark_aloise.hdf5
    - data/algorithms_in/clustering/benchmark_aloise/metadata.json
    """
    
    path_data_in = Path(".") / "data" / "src_datasets" / "clustering" / "benchmark_aloise"
    path_data_out = Path(".") / "data" / "algorithms_in" / "clustering" / "benchmark_aloise"
    path_data_out.parent.mkdir(parents=True, exist_ok=True)
    

    # Parse all in data
    data = {}
    metadata = {}
    for f in path_data_in.iterdir():
        if f.is_file() and f.suffix == ".dat":
            # read each .dat file
            data[f.stem] = []
            with open(f, "r") as fp:
                c = fp.readlines()
                # metadata: number of points and number of dimensions
                [n_pts, n_dims] = [int(e) for e in re.split("\t| ", c[0].strip())]
                metadata[f.stem] = {"NUM_DIM": n_dims, "NUM_POINTS": n_pts}
                # and the points
                data[f.stem] = [[float(a) for a in re.split("\t+| +", e.strip())] for e in c[1:]]
            data[f.stem] = np.array( data[f.stem])
            n_pts_found, n_dims_found = data[f.stem].shape
            # check that the metadata are correct
            assert (
                n_dims == n_dims_found
            ), f"Not correct number of dims, expected {n_dims}, found {n_dims_found}"
            assert (
                n_pts == n_pts_found
            ), f"Not correct number of points, expected {n_pts}, found {n_pts_found}"
    # Write to hdf5 the points and the metadata
    with File(path_data_out / "benchmark_aloise.hdf5", "w") as f:
        groups = {g: None for g in ["points", "dims", "num_clusters"]}
        for g in groups:
            groups[g] = f.create_group(g) # type: ignore
        for i, (inst, d) in enumerate(data.items()):
            for g, inp in zip(
                groups, [d, metadata[inst]["NUM_DIM"], metadata[inst]["NUM_POINTS"]]
            ):
                groups[g].create_dataset( # type: ignore
                    name=str(i), dtype="f", data=np.array(inp).flatten()
                )
            metadata[inst]["ID"] = i
    # Write the mapping
    with open(path_data_out / "metadata.json", "w") as fp:
        json.dump(metadata, fp, indent=4)