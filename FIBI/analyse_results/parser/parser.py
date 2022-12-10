from FIBI.analyse_results.parser.__init__ import *
from FIBI.analyse_results.parser.modifiers import AbstractModifier
from FIBI.analyse_results.utils.progressBars import DefaultProgressBar
from FIBI.analyse_results.parser.filters import FilterParsedRun

class AbstractBaseParser(abc.ABC):
    @abc.abstractmethod
    def call(self, key: str, filename: str, values: np.ndarray) -> dict:
        """Convert one run output (array of metedata and metrics) and converts it to a dictionnary."""
        pass
    def __call__(self, key: str, filename: str, values: np.ndarray) -> dict:
        return self.call(key, filename, values)



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

    def call(self, key: str, filename: str, values: np.ndarray) -> dict:
        """Convert one run output (array of metedata and metrics) and converts it to a dictionnary. Output contains at least the KEY field, the key in the hdf5 file"""
        dico = {
            "KEY":key,"filename":filename}
        for i,v in enumerate(values):
            name_metadata: str = self.mapping[i]
            value = float(v)
            dico[name_metadata] = value
        return dico
        
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
            with File(p, "r") as f:
                total_size += len(f['metadata']) # type: ignore
        # Parse the runs
        with DefaultProgressBar() as progress:
            progress.set_total_steps(total_size)
            for p in path_in:
                with File(p, "r") as f:
                    for k, v in f["metadata"].items():# type: ignore
                        dico = self.base_parser(k, p.stem, np.array(v))
                        keep = True
                        for filter_run in filters:
                            keep = keep and filter_run.filter_before_modifiers(dico)
                        if not keep:
                            continue
                        for m in modifiers:
                            dico = m(dico)
                        keep = True
                        for filter_run in filters:
                            keep = keep and filter_run.filter_after_modifiers(dico)
                        if not keep:
                            continue
                        L.append(dico)
                        progress.next(description=f"{p.stem}/{k}")
        return L