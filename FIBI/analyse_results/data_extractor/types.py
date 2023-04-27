from FIBI.analyse_results.data_extractor.__init__ import *
class ConsumerInStats(TypedDict):
    name: str
    query: dict
    
class DfExtract(TypedDict):
    """Container for extracted dataframe"""

    dico_keys: Dict[str, Union[float, str, int]]
    """keys used to extract this dataframe from the global metadata dataframe"""

    df: pd.DataFrame
    """dataframe of the extracted elements"""