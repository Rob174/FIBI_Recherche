"""Operations that allow to add/modify the parsed data from the hdf5 file. To be used with the parser module."""
from FIBI.analyse_results.parser.__init__ import *

class AbstractModifier(abc.ABC):
    """Object used to modify the results of a parsed hdf5 file."""
    @abc.abstractmethod
    def call(self, dico: dict) -> dict:
        """Modify the results of a parsed hdf5 file.
        # Args:
            dico: Dictionary containing one record of the resutls of a parsed hdf5 file.
        """
    def __call__(self, dico: dict) -> dict:
        return self.call(dico)
    
class ModifierIntMapping(AbstractModifier):
    """Map an integer attribute to a new value.
    
    # Args:
        name: Name of the attribute to map from.
        mapping: Mapping to use. Can be a path to a json file or a dictionary with first the expected key in the hdf5 file and then the value to map to.
        new_name: Name of the new attribute. If None, the name of the attribute to map from is used.
    """
    def __init__(self,name: str, mapping: Union[Path,Dict[int,Union[str,int]]], new_name: Optional[str] = None):
        self.name = name
        if isinstance(mapping,Path):
            with open(mapping,'r') as f:
                mapping = json.load(f)
        self.mapping: Dict[int,Union[str,int]] = mapping # type: ignore
        if new_name is None:
            new_name = self.name
        self.new_name = new_name
    def call(self,dico: dict):
        """Modify the results of a parsed hdf5 file, mapping an integer attribute to a new value."""
        dico[self.new_name] = self.mapping[dico[self.name]]
        return dico
class ModifierOperation(AbstractModifier):
    """Perform an operation on the results of a parsed hdf5 file.
    
    # Args:
        dst_name: Name of the attribute to store the result of the operation.
        operation: Operation to perform. Must take a dictionary as input and return a float or an int.
    """
    def __init__(self,dst_name: str, operation: Callable[[dict],Union[float,int,str]]):
        self.dst_name = dst_name
        self.operation = operation
    def call(self,dico: dict):
        """Modify the results of a parsed hdf5 file, performing the specified operation on it."""
        dico[self.dst_name] = self.operation(dico)
        return dico
class ConvertToInteger(AbstractModifier):
    """Perform an operation on the results of a parsed hdf5 file.
    
    # Args:
        dst_name: Name of the attribute to store the result of the operation.
        operation: Operation to perform. Must take a dictionary as input and return a float or an int.
    """
    def __init__(self,fields: List[str]):
        self.fields = fields
    def call(self,dico: dict):
        """Modify the results of a parsed hdf5 file, performing the specified operation on it."""
        for f in self.fields:
            dico[f] = int(dico[f])
        return dico
class Glob_SEEEDMaker(AbstractModifier):
    """Create a new attribute containing the seed of the SEEED algorithm.
    
    # Args:
        seed_name: Name of the attribute to store the seed.
    """
    def __init__(self,variating_params: List[str]):
        self.variating_params = variating_params
        self.seeds = {}
        self.id = 0
    def call(self,dico: dict):
        """Modify the results of a parsed hdf5 file, creating a new attribute containing the seed of the SEEED algorithm."""
        params = ','.join([str(dico[param]) for param in self.variating_params])
        if params not in self.seeds:
            self.seeds[params] = self.id
            self.id += 1
        dico['SEED_GLOB'] = int(self.seeds[params])
        return dico
