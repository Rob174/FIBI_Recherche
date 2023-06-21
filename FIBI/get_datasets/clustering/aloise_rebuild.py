from pathlib import Path
import pandas as pd
import numpy as np
from typing import *
import abc

Points = np.ndarray
NumClusters = int
NumDims = int
class AbstractGetter(abc.ABC):
    def __call__(self, path: Path) -> Tuple[Points,NumClusters,NumDims] :
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
    """
    def __call__(self, root: Path) -> Tuple[Points,NumClusters,NumDims] :
        path = root / "glass+identification" / "glass.data"
        cols = [
            "Id", "RI", "Na", "Mg", "Al", "Si", "K", "Ca", "Ba", "Fe", "Class"
        ]
        df = pd.read_csv(path,sep=",",header=None,names=cols)
        clusters = df['Class'].unique()
        attrs = df[cols[1:-1]].to_numpy()
        return attrs, len(clusters), attrs.shape[1]

class ImageSegmentation(AbstractGetter):
    
    """ Attributes description of the file
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
    def __call__(self, root: Path) -> Tuple[Points,NumClusters,NumDims] :
        path = root / "image+segmentation"/"segmentation.data"
        cols = [
            "Class",
            "region-centroid-col", "region-centroid-row", "region-pixel-count",
            "short-line-density-5", "short-line-density-2", "vedge-mean",
            "vedge-sd", "hedge-mean", "hedge-sd", "intensity-mean",
            "rawred-mean", "rawblue-mean", "rawgreen-mean", "exred-mean",
            "exblue-mean", "exgreen-mean", "value-mean", "saturatoin-mean",
            "hue-mean"
        ]
        with open(path) as f:
            file = [{k:v for k,v in zip(cols,e.strip().split(','))} for e in f.readlines()[5:]] # delete the first five rows to be able to parse
        df = pd.DataFrame(file)
        clusters = df['Class'].unique()
        attrs = df[cols[1:-1]].to_numpy()
        return attrs, len(clusters), attrs.shape[1]
    
class Ionosphere(AbstractGetter):
    
    """ Attributes description of the file
    Number of Attributes: 34 plus the class attribute
   -- All 34 predictor attributes are continuous
   """
    def __call__(self, root: Path) -> Tuple[Points,NumClusters,NumDims] :
        path = root / "ionosphere"/"ionosphere.data"
        df = pd.read_csv(path,sep=",",header=None)
        clusters = df[len(df.columns)-1].unique()
        attrs = df[list(range(len(df.columns)-1))].to_numpy()
        return attrs, len(clusters), attrs.shape[1]
class Iris(AbstractGetter):
    """ Attributes description of the file
        1. sepal length in cm
        2. sepal width in cm
        3. petal length in cm
        4. petal width in cm
        5. class: 
            -- Iris Setosa
            -- Iris Versicolour
            -- Iris Virginica
    """
    def __call__(self, root: Path) -> Tuple[Points,NumClusters,NumDims] :
        path = root / "iris"/ "iris.data"
        cols = [
            "sepal-length", "sepal-width", "petal-length", "petal-width", "class"
        ]
        df = pd.read_csv(path,sep=",",header=None,names=cols)
        clusters = df[cols[-1]].unique()
        attrs = df[cols[:-1]].to_numpy()
        return attrs, len(clusters), attrs.shape[1]
if __name__ == "__main__":
    root = Path("data/src_datasets/clustering/benchmark_aloise/rebuild/src/")
    Iris()(root)