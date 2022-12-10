"""
Takes the output of the c++ algorithms and generates a list of dict of all of the data (metadata and metrics) of all of the runs.
AbstractModifier allows to add/modify data with a visibility of only one run at a time.
```txt
                            ┌
                            |  Metadata         xxxxxx
                            |                  xx    xxxx
                         ┌─►| SEED_PROBLEM     xx      xx
                         │  | ...              │xxxxxxxx│
                         │  | final_cost       │        │
                 \\\     │  └                  │        │
       ─────────── \\    │                     │  HDF5  │
      C++ program   //  Generates  ──────────► │  files │
       ─────────── //                          │        │
                 ///                           │        │
                                               xxx    xxx
                                                 xxxxxx
                  ┌─────────────────────────┐       │
                  │          MainParser     │       │
                  │   ( AbstractBaseParser) │     ┌─0─┐        AbstractModifier
                  │ parser for which problem│ ────┘ │ └──  (to add/modify columns)
                  └─────────────────────────┘       │
                                                    │ List[dict]       
```
"""

from FIBI.analyse_results.__init__ import *
from h5py import File