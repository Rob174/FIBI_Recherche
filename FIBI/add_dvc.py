import pathlib
import argparse
from typing import *
from rich.progress import track
import os
from multiprocessing import Pool

def process_file(file_path, add, remove,i,tot):
    print(f"{i}/{tot}: {i/tot*100:2f}%")
    if file_path.is_file() and file_path.suffix != ".py":
        if add:
            # Add the file to the directory
            os.system("dvc add "+file_path.resolve().as_posix())
        elif remove:
            # Remove the file from the directory
            # os.system("dvc remove"+file_path.resolve().as_posix())
#
def dvc_manager(add_rmv: Optional[Literal['add','rmv']] = None, folder_path: Optional[pathlib.Path] = None):
    parser = argparse.ArgumentParser(description='Add or remove files in a directory.')
    if folder_path is None:
        parser.add_argument('folder_path', type=pathlib.Path, help='Path to the directory', default="./data/")
    if add_rmv is None:
        parser.add_argument('-a', '--add', action='store_true', help='Add files to the directory')
        parser.add_argument('-r', '--remove', action='store_true', help='Remove files from the directory')

    args = parser.parse_args()
    if folder_path is None:
        folder_path = args.folder_path
    if add_rmv is None:
        add = args.add
        remove = args.remove
        if not args.add and not args.remove:
            parser.error('You must choose either --add or --remove.')

        if args.add and args.remove:
            parser.error('You cannot use both --add and --remove together.')
    else:
        add = add_rmv == "add"
        remove = add_rmv == "rmv"
    files = list(folder_path.rglob('*'))
    tot = len(files)
    with Pool(processes=16) as pool:
        pool.starmap(process_file, [(file_path, add, remove, i,tot) for i,file_path in enumerate(files)])

if __name__ == "__main__":
    dvc_manager("rmv",pathlib.Path("data/"))