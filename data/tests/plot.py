from h5py import File
import numpy as np
import plotly.express as px

if __name__ == "__main__":
    with File('data/tests/20points_16clusters_RAND_sample1.hdf5') as f:
        points = {k:np.copy(v).reshape(-1,2) for k,v in f['points_coords'].items()}
        clusters = {k:[str(e) for e in np.copy(v)] for k,v in f['final_assignements'].items()}
    for k in points:
        if '6001' in k:
            title = 'BICL'
        else:
            title = 'FICL'
        px.scatter(x=points[k][:,0],y=points[k][:,1],opacity=0,text=clusters[k],color_discrete_sequence=px.colors.qualitative.Plotly,title=title).show()