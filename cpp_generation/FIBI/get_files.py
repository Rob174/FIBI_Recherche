from pathlib import Path
from pyperclip import copy
from itertools import chain

if __name__ == "__main__":
    root = Path('.')
    print(root.resolve())
    # ITerate over all files and files in subfolders that have extension .h .cpp .hpp
    s = ''
    for file in chain(root.glob('**/*.h'), root.glob('**/*.cpp'), root.glob('**/*.hpp')):
        # check that build folder is not in the path
        if 'build' in str(file):
            continue
        print(file)
        s += '"./'+str(file.as_posix())+'"' + '\n'
    copy(s)
    