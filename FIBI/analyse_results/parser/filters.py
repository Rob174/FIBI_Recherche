"""Allow to remove some runs from the hdf5."""

from FIBI.analyse_results.parser.__init__ import *

class FilterParsedRun(abc.ABC):
    def filter_before_modifiers(self, dico: dict) -> bool:
        """Return True if the run should be kept, False otherwise."""
        return True
    def filter_after_modifiers(self, dico: dict) -> bool:
        """Return True if the run should be kept, False otherwise."""
        return True
    
class FilterAttrValueInt(FilterParsedRun):
    """See if attr value is in values_to_keep. If not the case, remove the run. Filters before the modifiers."""
    def __init__(self, attr: str, values_to_keep: List[int], after: bool = False) -> None:
        super().__init__()
        self.attr = attr
        self.values_to_keep = values_to_keep
        self.after = after
    def filter_dico(self, dico: dict) -> bool:
        return int(dico[self.attr]) in self.values_to_keep
    def filter_before_modifiers(self, dico: dict) -> bool:
        if self.after:
            return True
        return self.filter_dico(dico)
    def filter_after_modifiers(self, dico: dict) -> bool:
        if not self.after:
            return True
        return self.filter_dico(dico)
class FilterDuplicatedKeys(FilterParsedRun):
    """Remove the runs with duplicated keys. Filters after the modifiers."""
    def __init__(self, use_filename: bool = True) -> None:
        super().__init__()
        self.keys = set({})
        self.use_filename = use_filename
    def filter_after_modifiers(self, dico: dict) -> bool:
        key = dico["KEY"]
        if self.use_filename:
            key += '_'+dico['filename']
        accept = key not in self.keys
        self.keys.add(key)
        return accept