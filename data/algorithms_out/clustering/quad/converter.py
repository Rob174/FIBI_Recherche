from h5py import File
import numpy as np
import json

if __name__ == "__main__":
    files = ["quad_kmeansplusplus"]
    with open('mapping.json') as f:
        old_mapping = json.load(f)
    with open('mapping_new.json') as f:
        new_mapping = json.load(f)
    for f_name in files:
        with File(f_name+".hdf5","r") as f_old:
            with File(f_name+"_new.hdf5","w") as f_new:
                g = f_new.create_group("metadata")
                for k,v in f_old['metadata'].items():
                    v = np.copy(v)
                    data = {}
                    for d in old_mapping:
                        data[d['name']] = v[int(d['id'])]
                    if data['IMPR'] == 2:
                        data['IMPR'] = 1
                    L = []
                    for d in sorted(new_mapping,key=lambda x: int(x['id'])):
                        L.append(data[d['name']])
                    g.create_dataset(k,data=np.array(L))