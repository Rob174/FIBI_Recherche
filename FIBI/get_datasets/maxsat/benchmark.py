from FIBI.analyse_results.utils.progressBars import *
from FIBI.get_datasets.maxsat.__init__ import *
from rich.console import Console
from rich.progress import track
from h5py import File
import json

console = Console()


def stem(s: str):
    path = Path(s)
    if path.stem == str(s):
        return str(path)
    else:
        return stem(path.stem)


i_seen = 0
i_hdf5 = 0
start = True


def on_end(progress):
    global i_seen, i_hdf5
    with open("status.json", "w") as f:
        json.dump({"i_seen": i_seen, "i_hdf5": i_hdf5}, f)
    progress.next(f"{f.name}")
    i_seen += 1


def on_start_skip():
    global i_seen, i_hdf5
    if Path("status.json").exists():
        if start:
            with open("status.json", "r") as f:
                status = json.load(f)
            if status["i_seen"] >= i_seen:
                return True
        return False
    else:
        with open("status.json", "w") as f:
            json.dump({"i_seen": -1, "i_hdf5": -1}, f)
        return False


def parse_file(f, progress, root_folder):
    global i_seen, i_hdf5
    skip = on_start_skip()
    if skip:
        on_end(progress)
        return
    if f.suffix == ".wcnf":
        on_end(progress)
        return
    progress.next(f"{f.name} ...", advance=0)
    # Setup pathes
    path_7zip = '"C:\\Program Files\\7-Zip\\7z.exe" e '
    path_exe = Path(
        "C:/Users/robin/Documents/Cours/Poly/Recherche/FIBI_Recherche/cpp_generation/parse_benchmark_maxsat/x64/Release/parse_benchmark_maxsat.exe"
    )
    path_out_wcnf = path_exe.parent / f"in.wcnf"
    # Setup to save the file name
    parents_datasets = "--".join(
        [
            p.stem
            for p in list(f.parents)
            if root_folder.stem in [pp.stem for pp in p.parents]
        ][::-1]
    )
    name = f"{parents_datasets}--{stem(f)}"
    # Extract the wcnf file
    command = f"{path_7zip} {f.resolve()} -so > {path_out_wcnf}"
    res = os.system(command)
    if res != 0:
        on_end(progress)
        return
    # If too big, skip
    if os.path.getsize(str(path_out_wcnf)) >= 1e9:
        print(
            "skipping ",
            name,
            f" with size {int(os.path.getsize(str(path_out_wcnf))):,}B",
        )
        os.remove(str(path_out_wcnf))
        on_end(progress)
        return
    current_path = Path(".").resolve()
    res = os.system(
        f"cd {path_out_wcnf.parent}&&{path_exe} -id {i_hdf5} -orig_name {name} -weighted"
    )
    # If the file does not satisfy the requirements, skip
    if res == 1:
        os.remove(str(path_out_wcnf))
        on_end(progress)
        return
    elif res > 1:
        raise Exception
    os.system(f"cd {current_path}")
    os.remove(str(path_out_wcnf))
    i_hdf5 += 1
    on_end(progress)


def parse_folder(f, progress, root_folder, first_only=False):
    if f.stem == "out_txt":
        return
    elements = list(f.iterdir())
    for content in elements:
        if content.is_file() and content.suffix == ".gz":
            parse_file(content, progress, root_folder)
            if first_only:
                break
        elif content.is_dir():
            parse_folder(content, progress, root_folder, first_only=first_only)


def get_num_elements_folder(f, first_only: bool = False) -> int:
    if f.stem == "out_txt":
        return 0
    elements = list(f.iterdir())
    num_elems = 0
    for content in elements:
        if content.is_file() and content.suffix == ".gz":
            num_elems += 1
            if first_only:
                break
        elif content.is_dir():
            num_elems += get_num_elements_folder(content, first_only=first_only)
    return num_elems


def unzip(path: Path):
    path_7zip = '"C:\\Program Files\\7-Zip\\7z.exe" x '
    for f in track(list(path.iterdir())):
        if f.suffix == ".zip":
            os.system(path_7zip + str(f.resolve()))


def maxsat_benchmark():
    # maxsat_benchmark
    root = Path("data/src_datasets/maxsat/maxsat_benchmark/")
    # clean the hdf5 file and stats.json if present
    exe_folder = Path(r"C:\Users\robin\Documents\Cours\Poly\Recherche\FIBI_Recherche\cpp_generation\parse_benchmark_maxsat\x64\Release")
    try:
        os.remove(exe_folder / "stats.json")
        print("removing stats.json")
    except FileNotFoundError:
        pass
    try:
        os.remove(exe_folder / "dataset.hdf5")
        print("removing dataset.hdf5")
    except FileNotFoundError:
        pass
    # Extraire les dossiers
    n = get_num_elements_folder(root)
    with DefaultProgressBar() as progress:
        progress.set_total_steps(n)
        parse_folder(root, progress, root)


def maxsat_familles():
    # maxsat_benchmark
    root = Path("data/src_datasets/maxsat/maxsat_51families/")
    # Extraire les dossiers
    n = get_num_elements_folder(root, first_only=True)
    with DefaultProgressBar() as progress:
        progress.set_total_steps(n)
        parse_folder(root, progress, root, first_only=True)

def main():
    
    print("maxsat_benchmark()")
    
    maxsat_benchmark()

if __name__ == "__main__":
    main()