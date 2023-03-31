from FIBI.analyse_results.visualization.__init__ import *

class HTMLDataOut(TypedDict):
    query: dict
    data: Any
    type: str
    name: str

class AggregatedData(TypedDict):
    type: str
    data: Any
    name:str
    classes: List[str]
    query: dict
class BufferDataAggr(TypedDict):
    query: dict
    fields: List[AggregatedData]