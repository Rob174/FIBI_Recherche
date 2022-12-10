from FIBI.analyse_results.utils.__init__ import *


class DicoPathConverter:
    def __init__(self, path_mapping: Path, overwrite: Optional[bool] = True) -> None:
        self.path_mapping = path_mapping
        if path_mapping.exists() and not overwrite:
            with open(path_mapping, "r") as f:
                self.dico = json.load(f)
        else:
            self.dico = []
        self.search_set = {}
        for i, d in enumerate(self.dico):
            self.search_set[d["key"]] = i
        self.id_dico = max(self.search_set.values()) + 1 if self.search_set else 0

    def to_path(self, dico: dict) -> str:
        """Convert a dico into a path using the format:

        key1{k_v_sep}value1{sep}key2{k_v_sep}value2{sep}...
        """
        key = str({k:dico[k] for k in sorted(dico)})
        d = {"key": key, **{k: str(v) for k, v in dico.items()}}
        if key in self.search_set:
            id = self.search_set[key]
        else:
            id = self.id_dico
            self.dico.append(d)
            self.search_set[key] = id
            self.id_dico += 1
        return str(id)

    def save(self):
        with open(self.path_mapping, "w") as f:
            json.dump(self.dico, f)


def dico_to_query(
    dico: Dict[str, Union[str, float, int]], sep=" & ", sep_attr_val="=="
) -> str:
    """
    Join the keys and values into a string:

    dico: Dict[str, Union[str, float, int]], of the form {"NAME_METADATA1":value_metadata1, ...}

    Ex:

    input:

    ```py
    dico_to_query({"NAME_METADATA1":"my_value", "NAME_METADATA":0}, sep=' & ')
    ```

    output:

    ```py
    'NAME_METADATA1=="value_metadata1" & NAME_METADATA2 == 0'
    ```

    """
    query_elems = []
    for k, v in dico.items():
        if isinstance(v, str):
            v = f'"{v}"'
        v = str(v)
        query_elems.append(k + sep_attr_val + v)
    return sep.join(query_elems)


def path_create(path: Path) -> Path:
    if path.suffix == "":
        path.mkdir(parents=True, exist_ok=True)
    else:
        path.parent.mkdir(parents=True, exist_ok=True)
    return path
