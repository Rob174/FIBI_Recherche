from pathlib import Path
import pandas as pd
import numpy as np
from typing import *
import abc
import re
from itertools import islice
import json
from io import StringIO

Points = np.ndarray
NumClusters = int
NumDims = int


class AbstractGetter(abc.ABC):
    def __call__(self, path: Path) -> Tuple[Points, NumClusters, NumDims]:
        """Extract the data from the files .data provided inside the folder path"""
        pass


class Glass(AbstractGetter):
    """
    Attributes description of the file
        1. Id number: 1 to 214
        2. RI: refractive index
        3. Na: Sodium (unit measurement: weight percent in corresponding oxide, as
                        are attributes 4-10)
        4. Mg: Magnesium
        5. Al: Aluminum
        6. Si: Silicon
        7. K: Potassium
        8. Ca: Calcium
        9. Ba: Barium
        10. Fe: Iron
        11. Type of glass: (class attribute)
            -- 1 building_windows_float_processed
            -- 2 building_windows_non_float_processed
            -- 3 vehicle_windows_float_processed
            -- 4 vehicle_windows_non_float_processed (none in this database)
            -- 5 containers
            -- 6 tableware
            -- 7 headlamps
        Note: Include the Id as was in the paper eventhough not really relevant...
        A class that is not present in this dataset....
        In the given file 9 attributes (exclude the id) with no class
    """

    def __call__(self, root: Path) -> Tuple[Points, NumClusters, NumDims]:
        print(
            "WARNING: Include the Id as was in the paper even though not really relevant..."
        )
        path = root / "glass+identification" / "glass.data"
        cols = ["Id", "RI", "Na", "Mg", "Al", "Si", "K", "Ca", "Ba", "Fe", "Class"]
        df = pd.read_csv(path, sep=",", header=None, names=cols)
        clusters = df["Class"].unique()
        attrs = df[cols[:-1]].to_numpy()
        return attrs, attrs.shape[0], 7, attrs.shape[1]


class ImageSegmentation(AbstractGetter):

    """Attributes description of the file
    1.  region-centroid-col:  the column of the center pixel of the region.
    2.  region-centroid-row:  the row of the center pixel of the region.
    3.  region-pixel-count:  the number of pixels in a region = 9.
    4.  short-line-density-5:  the results of a line extractoin algorithm that
         counts how many lines of length 5 (any orientation) with
         low contrast, less than or equal to 5, go through the region.
    5.  short-line-density-2:  same as short-line-density-5 but counts lines
         of high contrast, greater than 5.
    6.  vedge-mean:  measure the contrast of horizontally
         adjacent pixels in the region.  There are 6, the mean and
         standard deviation are given.  This attribute is used as
        a vertical edge detector.
    7.  vegde-sd:  (see 6)
    8.  hedge-mean:  measures the contrast of vertically adjacent
          pixels. Used for horizontal line detection.
    9.  hedge-sd: (see 8).
    10. intensity-mean:  the average over the region of (R + G + B)/3
    11. rawred-mean: the average over the region of the R value.
    12. rawblue-mean: the average over the region of the B value.
    13. rawgreen-mean: the average over the region of the G value.
    14. exred-mean: measure the excess red:  (2R - (G + B))
    15. exblue-mean: measure the excess blue:  (2B - (G + R))
    16. exgreen-mean: measure the excess green:  (2G - (R + B))
    17. value-mean:  3-d nonlinear transformation
         of RGB. (Algorithm can be found in Foley and VanDam, Fundamentals
         of Interactive Computer Graphics)
    18. saturatoin-mean:  (see 17)
    19. hue-mean:  (see 17)"""

    def __call__(self, root: Path) -> Tuple[Points, NumClusters, NumDims]:
        path1 = root / "image+segmentation" / "segmentation.data"
        path2 = root / "image+segmentation" / "segmentation.test"
        cols = [
            "Class",
            "region-centroid-col",
            "region-centroid-row",
            "region-pixel-count",
            "short-line-density-5",
            "short-line-density-2",
            "vedge-mean",
            "vedge-sd",
            "hedge-mean",
            "hedge-sd",
            "intensity-mean",
            "rawred-mean",
            "rawblue-mean",
            "rawgreen-mean",
            "exred-mean",
            "exblue-mean",
            "exgreen-mean",
            "value-mean",
            "saturatoin-mean",
            "hue-mean",
        ]

        def to_df(path) -> pd.DataFrame:
            with open(path) as f:
                file = [
                    {k: v for k, v in zip(cols, e.strip().split(","))}
                    for e in f.readlines()[5:]
                ]  # delete the first five rows to be able to parse
            df = pd.DataFrame(file)
            clusters = df["Class"].unique()
            attrs = df[cols[1:]].to_numpy()
            return df, clusters, attrs

        df1, clusters1, attrs1 = to_df(path1)
        df2, clusters2, attrs2 = to_df(path2)
        attrs = np.concatenate([attrs1, attrs2], axis=0)
        return (
            attrs,
            attrs.shape[0],
            len(set(clusters1).union(set(clusters2))),
            attrs1.shape[1],
        )


class Ionosphere(AbstractGetter):

    """Attributes description of the file
     Number of Attributes: 34 plus the class attribute
    -- All 34 predictor attributes are continuous
    """

    def __call__(self, root: Path) -> Tuple[Points, NumClusters, NumDims]:
        path = root / "ionosphere" / "ionosphere.data"
        df = pd.read_csv(path, sep=",", header=None)
        clusters = df[len(df.columns) - 1].unique()
        attrs = df[list(range(len(df.columns) - 1))].to_numpy()
        return attrs, attrs.shape[0], len(clusters), attrs.shape[1]


class Iris(AbstractGetter):
    """Attributes description of the file
    1. sepal length in cm
    2. sepal width in cm
    3. petal length in cm
    4. petal width in cm
    5. class:
        -- Iris Setosa
        -- Iris Versicolour
        -- Iris Virginica
    """

    def __call__(self, root: Path) -> Tuple[Points, NumClusters, NumDims]:
        path = root / "iris" / "iris.data"
        cols = ["sepal-length", "sepal-width", "petal-length", "petal-width", "class"]
        df = pd.read_csv(path, sep=",", header=None, names=cols)
        clusters = df[cols[-1]].unique()
        attrs = df[cols[:-1]].to_numpy()
        return attrs, attrs.shape[0], len(clusters), attrs.shape[1]


class LibrasMovement(AbstractGetter):
    """Attributes description of the file
    1. 1� coordinate abcissa
    2. 1� coordinate ordinate
    3. 2� coordinate abcissa
    4. 2� coordinate ordinate
    ...
    89. 45� coordinate abcissa
    90. 45� coordinate ordinate
    91. class:
        -- 1: curved swing
        -- 2: horizontal swing
        -- 3: vertical swing
        -- 4: anti-clockwise arc
        -- 5: clockwise arc
        -- 6: circle
        -- 7: horizontal straight-line
        -- 8: vertical straight-line
        -- 9: horizontal zigzag
        -- 10: vertical zigzag
        -- 11: horizontal wavy
        -- 12: vertical wavy
        -- 13: face-up curve
        -- 14: face-down curve
        -- 15: tremble
    """

    def __call__(self, root: Path) -> Tuple[Points, NumClusters, NumDims]:
        path = root / "libras+movement" / "movement_libras.data"
        cols = list(range(91))
        df = pd.read_csv(path, sep=",", header=None, names=list(range(91)))
        clusters = df[cols[-1]].unique()
        attrs = df[cols[:-1]].to_numpy()
        return attrs, attrs.shape[0], len(clusters), attrs.shape[1]


class SyntheticControlChart(AbstractGetter):
    """Attributes description of the file
    The data is stored in an ASCII file, 600 rows, 60 columns, with a single chart per line. The classes are organized as follows:
    1-100   Normal
    101-200 Cyclic
    201-300 Increasing trend
    301-400 Decreasing trend
    401-500 Upward shift
    501-600 Downward shift
    No class columns we need to infer from the line number above
    Separator is " +" in regex
    """

    def __call__(self, root: Path) -> Tuple[Points, NumClusters, NumDims]:
        path = root / "synthetic+control+chart+time+series" / "synthetic_control.data"
        # read
        with open(path) as f:
            file = [list(map(float, re.split(" +", e))) for e in f.readlines()]
        intervals = [i * 100 for i in range(7)]
        for i in range(len(file)):
            for _class, (start, end) in enumerate(
                zip(
                    intervals, [intervals[i + 1] - 1 for i in range(len(intervals) - 1)]
                )
            ):
                if i >= start and i <= end:
                    file[i] = np.array(file[i] + [_class])
                    break
        data = np.stack(file, axis=0)
        # extract classes
        clusters = np.unique(data[:, -1])
        attrs = data[:, :-1]
        return attrs, attrs.shape[0], len(clusters), attrs.shape[1]


class Wine(AbstractGetter):
    """Attributes description of the file
    1) Alcohol
    2) Malic acid
    3) Ash
    4) Alcalinity of ash
    5) Magnesium
    6) Total phenols
    7) Flavanoids
    8) Nonflavanoid phenols
    9) Proanthocyanins
    10)Color intensity
    11)Hue
    12)OD280/OD315 of diluted wines
    13)Proline
    """

    def __call__(self, root: Path) -> Tuple[Points, NumClusters, NumDims]:
        path = root / "wine" / "wine.data"
        # read
        df = pd.read_csv(path, sep=",", header=None)
        attrs = df[df.columns[1:]].to_numpy()
        return attrs, attrs.shape[0], 3, attrs.shape[1]


class Yeast(AbstractGetter):
    """Extracted from the files provided in the zip"""

    def __call__(self, root: Path) -> Tuple[Points, NumClusters, NumDims]:
        path = root / ".." / ".." / "old" / "yeast1484.dat"
        # read
        with open(path) as f:
            lines = [
                [float(a) for a in l.split(" ")]
                for i, l in enumerate(f.readlines())
                if i > 0
            ]
        df = pd.DataFrame(lines)
        attrs = df.to_numpy()
        return attrs, attrs.shape[0], 10, attrs.shape[1]


class Yeast(AbstractGetter):
    """Extracted from the files provided in the zip"""

    def __call__(self, root: Path) -> Tuple[Points, NumClusters, NumDims]:
        path = root / ".." / ".." / "old" / "yeast1484.dat"
        # read
        with open(path) as f:
            lines = [
                [float(a) for a in l.split(" ")]
                for i, l in enumerate(f.readlines())
                if i > 0
            ]
        df = pd.DataFrame(lines)
        attrs = df.to_numpy()
        return attrs, attrs.shape[0], 10, attrs.shape[1]


class BodyMeasurement(AbstractGetter):
    """Extracted from the files provided in the zip"""

    def __call__(self, root: Path) -> Tuple[Points, int, NumClusters, NumDims]:
        path = root / ".." / ".." / "old" / "body507_s5.dat"
        # read
        with open(path) as f:
            lines = [
                [float(a) for a in l.split("\t") if a != ""]
                for i, l in enumerate(f.readlines())
                if i > 0
            ]
        df = pd.DataFrame(lines)
        attrs = df.to_numpy()
        return attrs, attrs.shape[0], 2, attrs.shape[1]


class Thyroid(AbstractGetter):
    """Extracted from the files provided in the zip"""

    def __call__(self, root: Path) -> Tuple[Points, int, NumClusters, NumDims]:
        path = root / ".." / ".." / "old" / "thyroid.txt"
        # read
        with open(path) as f:
            lines = [
                [float(a) for a in l.split(" ") if a != ""]
                for i, l in enumerate(f.readlines())
            ]
        df = pd.DataFrame(lines)
        attrs = df.to_numpy()
        return attrs, attrs.shape[0], 3, attrs.shape[1]


class UserKnowledge(AbstractGetter):
    """Extracted from the files provided in the zip"""

    def __call__(self, root: Path) -> Tuple[Points, int, NumClusters, NumDims]:
        path = root / ".." / ".." / "old" / "user_knowledge.txt"
        # read
        with open(path) as f:
            lines = [
                [float(a) for a in l.split(" ") if a.strip() != ""]
                for i, l in enumerate(f.readlines())
            ]
        df = pd.DataFrame(lines)
        attrs = df.to_numpy()
        return attrs, attrs.shape[0], 4, attrs.shape[1]


class WaterTreatment(AbstractGetter):
    """Extracted from the files provided in the zip"""

    def __call__(self, root: Path) -> Tuple[Points, int, NumClusters, NumDims]:
        path = root / ".." / ".." / "old" / "water.txt"
        # read
        with open(path) as f:
            lines = [
                [float(a) for a in l.split(" ") if a != ""]
                for i, l in enumerate(f.readlines())
            ]
        df = pd.DataFrame(lines)
        attrs = df.to_numpy()
        return attrs, attrs.shape[0], 13, attrs.shape[1]


class BreastCancer(AbstractGetter):
    """Extracted from the files provided in the zip"""

    def __call__(self, root: Path) -> Tuple[Points, int, NumClusters, NumDims]:
        path = root / ".." / ".." / "old" / "breast_cancer.txt"
        # read
        with open(path) as f:
            lines = [
                [float(a) for a in l.split(" ") if a != ""]
                for i, l in enumerate(f.readlines())
            ]
        df = pd.DataFrame(lines)
        attrs = df.to_numpy()
        return attrs, attrs.shape[0], 2, attrs.shape[1]


class Vehicle(AbstractGetter):
    """Extracted from the files provided in the zip"""

    def __call__(self, root: Path) -> Tuple[Points, int, NumClusters, NumDims]:
        path = root / ".." / ".." / "old" / "vehicle.txt"
        # read
        with open(path) as f:
            lines = [
                [float(a) for a in l.split(" ") if a != ""]
                for i, l in enumerate(f.readlines())
            ]
        df = pd.DataFrame(lines)
        attrs = df.to_numpy()
        return attrs, attrs.shape[0], 6, attrs.shape[1]


class Vowel(AbstractGetter):
    """Extracted from the files provided in the zip"""

    def __call__(self, root: Path) -> Tuple[Points, int, NumClusters, NumDims]:
        print("WARNING n attr different from paper")
        path = root / ".." / ".." / "old" / "vowel.txt"
        # read
        with open(path) as f:
            lines = [
                [float(a) for a in l.split(" ") if a != ""]
                for i, l in enumerate(f.readlines())
            ]
        df = pd.DataFrame(lines)
        attrs = df.to_numpy()
        return attrs, attrs.shape[0], 11, attrs.shape[1]


class MultipleFeatures(AbstractGetter):
    """Extracted from the files provided in the zip"""

    def __call__(self, root: Path) -> Tuple[Points, int, NumClusters, NumDims]:
        path = root / ".." / ".." / "old" / "multiple_featuresReduced.txt"
        # read
        with open(path) as f:
            lines = [
                [float(a) for a in l.split(" ") if a != ""]
                for i, l in enumerate(f.readlines())
            ]
        df = pd.DataFrame(lines)
        attrs = df.to_numpy()
        return attrs, attrs.shape[0], 7, attrs.shape[1]

class Balance(AbstractGetter):
    """	
    1. Class Name: 3 (L, B, R)
	2. Left-Weight: 5 (1, 2, 3, 4, 5)
	3. Left-Distance: 5 (1, 2, 3, 4, 5)
	4. Right-Weight: 5 (1, 2, 3, 4, 5)
	5. Right-Distance: 5 (1, 2, 3, 4, 5)
    """

    def __call__(self, root: Path) -> Tuple[Points, int, NumClusters, NumDims]:
        path = root / "balance+scale" / "balance-scale.data"
        # read
        df = pd.read_csv(path,sep=",",header=None)[[1,2,3,4]]
        attrs = df.to_numpy()
        return attrs, attrs.shape[0], 3, attrs.shape[1]

class Optical(AbstractGetter):
    """	
    """

    def __call__(self, root: Path) -> Tuple[Points, int, NumClusters, NumDims]:
        path = root / "optical" / "data_x.npy"
        attrs = np.load(path).astype(np.float32)
        return attrs, attrs.shape[0], 10, attrs.shape[1]
class Cardiotocography(AbstractGetter):
    """	
    """

    def __call__(self, root: Path) -> Tuple[Points, int, NumClusters, NumDims]:
        path = root / "cardiotocography" / "CTG.txt"
        # read
        with open(path,"r") as f:
            data = f.read().replace(",",".")
        df = pd.read_csv(StringIO(data),sep="\t",header=0)[list(map(str,range(1,24)))]
        # delete first and 3 last lines
        df = df.drop(df.index[[0,-1,-2,-3]])
        attrs = df.astype(float).to_numpy()
        return attrs, attrs.shape[0], 10, attrs.shape[1]
if __name__ == "__main__":
    root = Path("data/src_datasets/clustering/benchmark_aloise/rebuild/src/")
    path_out = Path("data/algorithms_in/clustering/benchmark_aloise/benchmark_aloise")
    path_out.mkdir(parents=True, exist_ok=True)
    classes = [
        Glass,
        ImageSegmentation,
        Ionosphere,
        Iris,
        LibrasMovement,
        SyntheticControlChart,
        Wine,
        Yeast,
        BodyMeasurement,
        Thyroid,
        UserKnowledge,
        WaterTreatment,
        BreastCancer,
        Vehicle,
        Vowel, # different from paper
        MultipleFeatures,
        Balance,
        Cardiotocography,
        Optical
    ]
    elems = [[*e()(root), e.__name__] for e in classes]
    folders = ["dims", "num_clusters", "points"]
    for i in range(len(folders)):
        folders[i] = path_out / folders[i]
        folders[i].mkdir(exist_ok=True, parents=True)
    Ldico = {}
    for i, (data, n_points, n_clust, n_dims, class_name) in enumerate(
        sorted(elems, key=lambda x: x[1])
    ):
        print(i, class_name, n_points, n_dims, n_clust)
        with open(folders[0] / f"{i}.txt", "w") as f:
            f.write(f"{n_dims}")
        with open(folders[1] / f"{i}.txt", "w") as f:
            f.write(f"{n_clust}")
        with open(folders[2] / f"{i}.txt", "w") as f:
            data = "\n".join(["\n".join([str(a) for a in d]) for d in data])
            f.write(f"{data}")
        Ldico[class_name] = {
            "NUM_DIM": n_dims,
            "NUM_CLUST": n_clust,
            "NUM_POINTS": n_points,
            "ID": i,
        }
    with open(path_out / "metadata.json", "w") as f:
        json.dump(Ldico, f, indent=4)
