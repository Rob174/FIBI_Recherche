export type Matrix = Array<Array<number>>;
export type Vector = Array<number>;
interface MatrixSizeType {
  m: number; // rows
  n: number; // cols
}
function log(...args) {
    args[1] = JSON.parse(JSON.stringify(args[1]))
    console.log(...args)
}
function argmax(arr: Array<number>) {
  // from https://stackoverflow.com/questions/11301438/return-index-of-greatest-value-in-an-array
  if (arr.length === 0) {
    return -1;
  }

  var max = arr[0];
  var maxIndex = 0;

  range(1, arr.length).forEach((i) => {
    if (arr[i] > max) {
      maxIndex = i;
      max = arr[i];
    }
  });
  return maxIndex;
}
/**
 *
 * @param {number} start
 * @param {number} stop excluded
 * @returns
 */
function range(start: number, stop: number) {
  if (start >= stop) {
    return [];
  } else {
    return Array(stop - start)
      .fill(0)
      .map((_, i) => i + start);
  }
}
/**
 *
 * @param {Matrix} A square matrix of real numbers
 * @param {Vector} b vector of real numbers
 * @returns {Vector|null} solution of the system or null if no solution or infinite number of solutions
 */
export function solve(A: Matrix, b: Vector): Vector | null {
  // from https://en.wikipedia.org/wiki/Gaussian_elimination
  const size = {
    m: b.length,
    n: A[0].length,
  };
  var h = 0;
  var k = 0;
  while (h < size.m-1 && k < size.n-1) {
    /* Find the k-th pivot: */
    const kCpy = k;
    const slice = A.slice(h, size.m).map((x) => Math.abs(x[kCpy])); // i_max = argmax (i = h ... m, abs(A[i, k]))
    const i_max = argmax(slice);
    if (i_max === -1) {
      return null;
    }
    if (A[i_max][k] === 0) {
      /* No pivot in this column, pass to next column */
      k++;
    } else {
      // swap rows(h, i_max)
      const rowH = [...A[h]];
      const rowImax = [...A[i_max]];
      const rowbH = b[h];
      const rowbImax = b[i_max];
      A[h] = rowImax;
      A[i_max] = rowH;
      b[h] = rowbImax;
      b[i_max] = rowbH;

      /* Do for all rows below pivot: */
      const arrIds = range(h + 1, size.m);
      for (let a = 0; a < arrIds.length; a++) {
        let i = arrIds[a];
        let f = A[i][k] / A[h][k];
        /* Fill with zeros the lower part of pivot column: */
        A[i][k] = 0;
        /* Do for all remaining elements in current row: */
        const arrIds2 = range(k + 1, size.n);
        for (let b = 0; b < arrIds2.length; b++) {
          let j = arrIds2[b];
          A[i][j] -= A[h][j] * f;
        }
        b[i] = b[i] - b[h] * f;
      }
    }
    /* Increase pivot row and column */
    h++;
    k++;
  }
  if (b.filter((x) => isNaN(x)).length > 0) {
    return null; // no solution
  }
  const finalRes = backSubstitution(A, b, size);
  if (finalRes.filter((x) => isNaN(x)).length > 0) {
    return null; // no solution
  }
  return finalRes
}
function backSubstitution(
  triangA: Matrix,
  b: Vector,
  size: MatrixSizeType
): Array<number> {
    
  let res = Array(size.m).fill(null);
  const idList1 = range(0,size.m).reverse()
  for (let a = 0; a < idList1.length ; a++) {
    let i = idList1[a]
    res[i] = b[i];
    const idList = range(i+1,size.n).reverse()
    for (let b = 0; b < idList.length ; b++) {
        let j = idList[b]
        res[i] -= triangA[i][j] * res[j];
    }
    res[i] /= triangA[i][i];
  }
  return res;
}

export interface CoordType {
    x: number;
    y: number;
  }
export interface LineType {
    start: CoordType;
    end: CoordType;
  }
export interface RectangleType {
    width: number;
    height: number;
    center: CoordType;
  }

export function intersectionLines(
    line1: LineType,
    line2: LineType
  ): { inters: CoordType; t1: number; t2: number } | null {
    const A = line1.start,
      B = line1.end,
      C = line2.start,
      D = line2.end;
    const computeCoords = (t1, t2) => {
      return {
        x: A.x * t1 + B.x * (1 - t1),
        y: A.y * t1 + B.y * (1 - t1),
      };
    };
    let FirstMember: Matrix = [
      [A.x - B.x, D.x - C.x],
      [A.y - B.y, D.y - C.y],
    ];
    let SecondMember: Vector = [D.x - B.x, D.y - B.y];
    const result = solve(FirstMember, SecondMember);
    if (result === null) {
      return null;
    }
    return {
      inters: computeCoords(result[0], result[1]),
      t1: result[0],
      t2: result[1],
    };
  }

  export function intersectionLineRect(
    line: LineType,
    rect: RectangleType
  ): Array<{ inters: CoordType; t1: number; t2: number; side: number }> {
    const coordVertRect: Array<CoordType> = [
      { x: rect.center.x - rect.width / 2, y: rect.center.y - rect.height / 2 },
      { x: rect.center.x - rect.width / 2, y: rect.center.y + rect.height / 2 },
      { x: rect.center.x + rect.width / 2, y: rect.center.y + rect.height / 2 },
      { x: rect.center.x + rect.width / 2, y: rect.center.y - rect.height / 2 },
    ];
    const edgesRect = [
      { start: coordVertRect[0], end: coordVertRect[1] },
      { start: coordVertRect[1], end: coordVertRect[2] },
      { start: coordVertRect[2], end: coordVertRect[3] },
      { start: coordVertRect[3], end: coordVertRect[0] },
    ];
    console.log('edgesRect:',edgesRect)
    let intersections: Array<{
      inters: CoordType;
      t1: number;
      t2: number;
      side: number;
    }> = [];
    edgesRect.forEach((lineRect, i) => {
      const inter = intersectionLines(line, lineRect);
      if (inter !== null) {
        const inters = { ...inter, side: i, line: line, lineRect: lineRect };
        intersections.push(inters);
      }
    });
    console.log(
      "rectEdges:",
      JSON.stringify(edgesRect.map((x) => [x.start.x, x.start.y]))
    );
    console.log("lineEdge:", JSON.stringify([
      [line.start.x, line.start.y],
      [line.end.x, line.end.y],
    ]));
    console.log(
      "interP:",
      JSON.stringify(intersections.map((x) => [x.inters.x, x.inters.y]))
    );
    return intersections;
  }