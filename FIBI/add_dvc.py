import pathlib
import argparse
from typing import *

def dvc_manager(add_rmv: Optional[Literal['add','rmv']] = None, folder_path: Optional[pathlib.Path] = None):
    parser = argparse.ArgumentParser(description='Add or remove files in a directory.')
    if folder_path is None:
        parser.add_argument('folder_path', type=pathlib.Path, help='Path to the directory', default="./data/")
    if add_rmv is None:
        parser.add_argument('-a', '--add', action='store_true', help='Add files to the directory', default=)
        parser.add_argument('-r', '--remove', action='store_true', help='Remove files from the directory')

    args = parser.parse_args()

    if not args.add and not args.remove:
        parser.error('You must choose either --add or --remove.')

    if args.add and args.remove:
        parser.error('You cannot use both --add and --remove together.')

    for file_path in args.folder_path.rglob('*'):
        if file_path.is_file() and file_path.suffix != ".py":
            if args.add:
                # Add the file to the directory
                print(f'Adding file: {file_path}')
            elif args.remove:
                # Remove the file from the directory
                print(f'Removing file: {file_path}')

if __name__ == "__main__":
    dvc_manager("add","data/")