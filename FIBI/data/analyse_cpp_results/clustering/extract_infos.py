from enum import Enum
import numpy as np


class DataCategory(Enum):
    def __str__(self):
        return str(self.value)
    SEED_GLOB = "SEED_GLOB"
    NUM_POINTS = "NUM_POINTS"
    CLUST_IMPR = "CLUST_IMPR"
    NUM_CLUST = "NUM_CLUST"
    SEED_POINTS = "SEED_POINTS"
    SEED_ASSIGN = "SEED_ASSIGN"
    IT_ORDER = "IT_ORDER"
    IMPR_CLASS = "IMPR_CLASS"
    POINTS_DISTR = "POINTS_DISTR"
    init_cost = "init_cost"
    final_cost = "final_cost"
    num_iter = "num_iter"
    num_iter_glob = "num_iter_glob"
    duration = "duration"


corresp_dataset = {0: "uniform", 1: "blobs", 2: "tsplib", 3: "generatedDistMatrix"}
corresp_impr = {0: "random", 1: "kmeans+"}
corresp_it_order = {0: "BACK", 1: "CURR", 2: "RANDOM"}
corresp_impr_class = {
    0: "BICL",
    1: "FICL",
}


def parse_key(key: str):
    key_l = key.split(",")

    return {
        str(DataCategory.SEED_GLOB): int(key_l[0]),
        str(DataCategory.SEED_POINTS): int(key_l[1]),
        str(DataCategory.NUM_POINTS): int(key_l[2]),
        str(DataCategory.POINTS_DISTR): corresp_dataset[int(key_l[3])],
        str(DataCategory.SEED_ASSIGN): int(key_l[4]),
        str(DataCategory.IMPR_CLASS): corresp_impr_class[int(key_l[5])],
        str(DataCategory.NUM_CLUST): int(key_l[6]),
        str(DataCategory.CLUST_IMPR): corresp_impr[int(key_l[7])],
        str(DataCategory.IT_ORDER): corresp_it_order[int(key_l[8])],
    }


def parse_metadata(metadata: np.ndarray):
    return {
        str(DataCategory.NUM_POINTS): int(metadata[0]),
        "NUM_DIM": int(metadata[1]),
        str(DataCategory.POINTS_DISTR): int(metadata[2]),
        str(DataCategory.NUM_CLUST): int(metadata[3]),
        str(DataCategory.CLUST_IMPR): corresp_impr[int(metadata[4])],
        str(DataCategory.IMPR_CLASS): corresp_impr_class[int(metadata[5])],
        str(DataCategory.IT_ORDER): corresp_it_order[int(metadata[6])],
        str(DataCategory.SEED_POINTS): int(metadata[7]),
        str(DataCategory.SEED_ASSIGN): int(metadata[8]),
        str(DataCategory.SEED_GLOB): int(metadata[9]),
        str(DataCategory.init_cost): float(metadata[10]),
        str(DataCategory.final_cost): float(metadata[11]),
        str(DataCategory.num_iter): int(metadata[12]),
        str(DataCategory.num_iter_glob): int(metadata[13]),
        str(DataCategory.duration): float(metadata[14]) * 1e-6,  # convert to seconds
    }
