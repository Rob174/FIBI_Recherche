from FIBI.analyse_results.visualization.__init__ import *
from typing import *

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
    
FieldType = Literal['duration','num_iter','num_iter_glob','init_cost','final_cost','ratio']
TestName = Literal['SignTest','WilcoxonTest','TTest','ZTest']
CaseType = Literal['A1','A2','B1','B2','B3','C1','C2']