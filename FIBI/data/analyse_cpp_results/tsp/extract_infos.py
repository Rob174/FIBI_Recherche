from enum import Enum
import numpy as np
class DataCategory(Enum):
    def __str__(self):
        return str(self.value)
    NUM_POINTS = "NUM_POINTS"
    DATASET = "DATASET"
    NUM_CLUST = "NUM_CLUST"
    SEED_POINTS = "SEED_POINTS"
    TOUR_ALGO = "TOUR_ALGO"
    SEED_ASSIGN = "SEED_ASSIGN"
    IT_ORDER = "IT_ORDER"
    IMPR_CLASS = "IMPR_CLASS"
    init_cost = "init_cost"
    final_cost = "final_cost"
    num_iter = "num_iter"
    num_iter_glob = "num_iter_glob"
    duration = "duration"
corresp_dataset = {
    0: "uniform",
    1: "blobs",
    2: "tsplib",
    3: "generatedDistMatrix"
}
corresp_tour_algo = {
    0: "randomTour",
    1: "improvedTour"
}
corresp_it_order = {
    0: "BACK",
    1: "CURR",
    2: "RANDOM"
}
corresp_impr_class = {
    0: "BICL",
    1: "FICL",
    2: "BINL",
    3: "FINL",
}
def parse_key(key:str):
    key_l = key.split(',')
    
    return {
        str(DataCategory.NUM_POINTS): int(key_l[0]),
        str(DataCategory.DATASET): corresp_dataset[int(key_l[1])],
        str(DataCategory.NUM_CLUST): int(key_l[2]),
        str(DataCategory.SEED_POINTS): int(key_l[3]),
        str(DataCategory.TOUR_ALGO): corresp_tour_algo[int(key_l[4])],
        str(DataCategory.SEED_ASSIGN): int(key_l[5]),
        str(DataCategory.IT_ORDER): corresp_it_order[int(key_l[6])],
        str(DataCategory.IMPR_CLASS): corresp_impr_class[int(key_l[7])]
    }
def parse_metadata(metadata:np.ndarray):
    return {
        str(DataCategory.SEED_POINTS): int(metadata[0]),
        str(DataCategory.SEED_ASSIGN): int(metadata[1]),
        str(DataCategory.NUM_CLUST): int(metadata[2]),
        str(DataCategory.NUM_POINTS): int(metadata[3]),
        str(DataCategory.IMPR_CLASS): corresp_impr_class[int(metadata[4])],
        str(DataCategory.IT_ORDER): corresp_it_order[int(metadata[5])],
        str(DataCategory.init_cost): float(metadata[6]),
        str(DataCategory.final_cost): float(metadata[7]),
        str(DataCategory.num_iter): int(metadata[8]),
        str(DataCategory.num_iter_glob): int(metadata[9]),
        str(DataCategory.duration): float(metadata[10])*1e-6 # convert to seconds
    }