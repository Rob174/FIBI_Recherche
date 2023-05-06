"""Allow to remove some runs from the hdf5."""

from FIBI.analyse_results.parser.__init__ import *

class FilterParsedRun(abc.ABC):
    def __init__(self) -> None:
        super().__init__()
        self.count = 0
    def filter_before_modifiers(self, dico: dict) -> bool:
        """Return True if the run should be kept, False otherwise."""
        return True
    def filter_after_modifiers(self, dico: dict) -> bool:
        """Return True if the run should be kept, False otherwise."""
        return True
    def print_filtered(self):
        print(str(self.count)+" filtered by "+self.__class__.__name__)
    
class FilterAttrValueInt(FilterParsedRun):
    """See if attr value is in values_to_keep. If not the case, remove the run. Filters before the modifiers."""
    def __init__(self, attr: str, values_to_keep: List[int], after: bool = False) -> None:
        super().__init__()
        self.attr = attr
        self.values_to_keep = values_to_keep
        self.after = after
        self.count = 0
    def filter_dico(self, dico: dict) -> bool:
        r = int(dico[self.attr]) in self.values_to_keep
        if r is False:
            self.count += 1
        return r
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
        self.count = 0
    def filter_after_modifiers(self, dico: dict) -> bool:
        key = dico["KEY"]
        if self.use_filename:
            key += '_'+dico['filename']
        accept = key not in self.keys
        if not accept:
            self.count += 1
        self.keys.add(key)
        return accept
class FilterDuplicatedKeysPerGroup(FilterParsedRun):
    """Remove the runs with duplicated keys per grouped elements. Filters after the modifiers."""
    def __init__(self, group_attrs: List[str]) -> None:
        super().__init__()
        self.keys = {}
        self.group_attrs = group_attrs
        self.count = 0
    def filter_before_modifiers(self, dico: dict) -> bool:
        group = ",".join([str(dico[k]) for k in self.group_attrs])
        if group not in self.keys:
            self.keys[group] = set({})
        key = dico["KEY"]
        accept = key not in self.keys[group]
        if not accept:
            self.count += 1
        self.keys[group].add(key)
        return accept
class FilterMetricsObserved(FilterParsedRun):
    """Remove the runs that do not have metrics.
    # args
        attr_metric: str, the attribute to check the presence of to determine if there are the metrics"""
    def __init__(self, attr_metric: str = "init_cost") -> None:
        super().__init__()
        self.attr_metric = attr_metric
        self.count = 0
    def filter_before_modifiers(self, dico: dict) -> bool:
        accept = self.attr_metric in dico
        if not accept:
            self.count += 1
        return accept
        

class CountDataInstance(FilterParsedRun):
    """Check the number of time the metadata have the attribute provided and raise an exception if no metric is found for some cases"""
    def __init__(self, attr_to_find: dict, attr_metric_detect: Optional[str] = "final_cost"):
        self.count = 0
        self.attr_to_find = attr_to_find
        self.attr_metric_detect = attr_metric_detect
        super().__init__()
    def filter_before_modifiers(self, dico: dict) -> bool:
        matches = True
        for k,v in self.attr_to_find.items():
            matches = matches and dico[k] == v
            if not matches:
                return True
        self.count += 1
        if self.attr_metric_detect not in dico:
            raise Exception("Expected to get metrics for "+", ".join([k+":"+str(v) for k,v in dico.items()]))
        return True