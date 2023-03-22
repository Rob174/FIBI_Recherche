from pathlib import Path
from h5py import File, Group
import numpy as np
from rich.progress import Progress, track,SpinnerColumn,TimeElapsedColumn 

def dump_in_folder_datasets(folder, dataset):
	folder = Path(folder)
	folder.mkdir(parents=True, exist_ok=True) 
	# progress bar with elapsed time and ETA
	progress = Progress(
		SpinnerColumn(),
		*Progress.get_default_columns(),
		TimeElapsedColumn(),
	)
	with progress:
		for k,v in progress.track(dataset.items(),total=len(dataset), description=f"Dumping {folder.name}"):
			v = np.array(v)
			with open (folder / f"{k}.txt", "w") as f:
				f.write("\n".join([str(i) for i in v]))
if __name__ == "__main__":
    root = Path(".")
    in_folder = root
    out_folder = root / "folder"
    out_folder.mkdir(parents=True, exist_ok=True)
    for file in in_folder.iterdir():
        if file.suffix == ".hdf5":
            # check if groups exist, if so create folder and iterate over datasets inside, parse them as numpy arrays np.savetxt them to a file in folder with same name as dataset
            # if no groups exist, iterate over datasets and parse them as numpy arrays np.savetxt them to a file in folder with same name as dataset
            dataset_folder = out_folder / file.stem
            dataset_folder.mkdir(parents=True, exist_ok=True)
            with File(file,"r") as f:
                first_key = list(f.keys())[0]
                if isinstance(f[first_key], Group):
                    for group in f:
                        group = str(group)
                        group_folder = dataset_folder / str(group)
                        group_folder.mkdir(parents=True, exist_ok=True)
                        dump_in_folder_datasets(group_folder, f[group])
                else:
                    dump_in_folder_datasets(dataset_folder, f)
        