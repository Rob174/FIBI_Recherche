from io import TextIOWrapper
from FIBI.analyse_results.parser.__init__ import *
from FIBI.analyse_results.parser.modifiers import AbstractModifier
from FIBI.analyse_results.utils.progressBars import DefaultProgressBar
from FIBI.analyse_results.parser.filters import FilterParsedRun

class AbstractBaseParser(abc.ABC):
    @abc.abstractmethod
    def call(self, key: str, filename: str, values: np.ndarray) -> dict:
        """Convert one run output (array of metedata and metrics) and converts it to a dictionnary."""
        pass
    def __call__(self, filename: str, data: Any) -> dict:
        return self.call(filename, data)
    @abc.abstractmethod
    def open_file(self, path: Path) -> File:
        """Open a file and return the file object"""
        pass
    @abc.abstractmethod
    def get_data(self, file: Any) -> Iterable:
        """Return the data to parse"""
        pass



class Parser(AbstractBaseParser):
    """Parser Responsability: convert an array of metrics and metadata to a dictionnary
    
    # Args:
        path_mapping: Path to a json file containing the mapping following the format of mapping argument
        mapping: Mapping to use. List of dictionaries with with the property name (metadata or metric) and the index in the array of values of the hdf5 file.
        
        Note: The user must specified either path_mapping or mapping, but not both.
    """
    def __init__(
        self,
        path_mapping: Optional[Path] = None,
        mapping: Optional[List[Dict[str, int]]] = None,
    ):
        assert (path_mapping is not None) != (mapping is not None)
        if path_mapping is not None:
            with open(path_mapping, "r") as f:
                mapping = json.load(f)
        assert mapping is not None
        self.mapping:Dict[int,str] = {int(d['id']):d['name'] for d in mapping}

    def call(self, filename: str, key_values: Tuple) -> dict:
        """Convert one run output (array of metedata and metrics) and converts it to a dictionnary. Output contains at least the KEY field, the key in the hdf5 file"""
        key, values = key_values
        values = np.array(values)
        dico = {
            "KEY":key,"filename":filename}
        for i,v in enumerate(values):
            name_metadata: str = self.mapping[i]
            value = float(v)
            dico[name_metadata] = value
        return dico
    def open_file(self, path: Path) -> File:
        """Open a file and return the file object"""
        return File(path, "r")
    def get_data(self, file: File) -> Iterable:
        """Return the data to parse"""
        return file["metadata"].items()

class JSONParser(AbstractBaseParser):
    """Parser Responsability: convert an array of metrics and metadata to a dictionnary
    """
    def __init__(
        self,
        *args, **kwargs
    ):
        pass

    def call(self, filename: str, dico: dict) -> dict:
        """Convert one run output (array of metedata and metrics) and converts it to a dictionnary. Output contains at least the KEY field, the key in the hdf5 file"""
        new_dico = {**dico, "filename":filename, "KEY": dico['SEED_GLOB']}
        return new_dico
    def open_file(self, path: Path) -> TextIOWrapper:
        """Open a file and return the file object"""
        return open(path, "r")
    def get_data(self, file: TextIOWrapper) -> Iterable:
        """Return the data to parse"""
        text = "[" + file.read()[:-2] + "]"
        return json.loads(text)

class MainParser:
    """Main parser entry point: parse a list of hdf5 files and return a list of dictionnaries.
    Concretely iterate over the files, iterate over the runs in the files, parse the runs and apply modifiers to obtain the final list of dictionnaries.
    
    # Args:
        base_parser: Base parser to use. Depends of the type of problem output to parse
    """
    def __init__(self, base_parser: AbstractBaseParser):
        self.base_parser = base_parser
    def __call__(
        self, path_in: List[Path], modifiers: Optional[List[AbstractModifier]] = None, filters: Optional[List[FilterParsedRun]] = None
    ) -> List[dict]:
        """Parse a list of hdf5 files and return a list of dictionnaries."""
        if modifiers is None:
            modifiers = []
        if filters is None:
            filters = []
        L = []
        # Determine the total number of runs to parse in all the files
        total_size = 0
        for p in path_in:
            with self.base_parser.open_file(p) as f:
                total_size += len(self.base_parser.get_data(f)) # type: ignore
        # Parse the runs
        with DefaultProgressBar() as progress:
            progress.set_total_steps(total_size)
            for p in path_in:
                with self.base_parser.open_file(p) as f:
                    for d in self.base_parser.get_data(f):# type: ignore
                        if len(d) == 0:
                            continue
                        dico = self.base_parser(p.stem, d)
                        progress.next(description=f"{p.stem}/{dico['SEED_GLOB']}")
                        keep = True
                        for filter_run in filters:
                            keep = keep and filter_run.filter_before_modifiers(dico)
                        if not keep:
                            continue
                        b=0
                        for m in modifiers:
                            dico = m(dico)
                        keep = True
                        for filter_run in filters:
                            keep = keep and filter_run.filter_after_modifiers(dico)
                        if not keep:
                            continue
                        L.append(dico)
        for f in filters:
            f.print_filtered()
        return L