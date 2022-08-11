import { extent } from 'd3'

/**
 * @param data
 */
export function main (data) {
  groupedValues(data)
  filteredValues(data)
  groupedValuesDiff(data)
  groupedValuesGreedyRandom(data)
  groupedValuesGreedyRandomDiff(data)
}
/**
 * @param data
 */
function getSources (data) {
  const sources = []
  data.forEach(function (d) {
    if (sources.indexOf(d.source) === -1) {
      sources.push(d.source)
    }
  })
  sources.sort()
  return sources
}
/**
 * @param data
 */
function getAlgorithms (data) {
  const algorithms = []
  data.forEach(function (d) {
    if (algorithms.indexOf(d.IMPR_CLASS) === -1) {
      algorithms.push(d.IMPR_CLASS)
    }
  })
  algorithms.sort()
  return algorithms
}
/**
 * @param data
 */
function getNPoints (data) {
  const nPoints = []
  data.forEach(function (d) {
    if (nPoints.indexOf(parseInt(d.NUM_POINTS)) === -1) {
      nPoints.push(parseInt(d.NUM_POINTS))
    }
  })
  nPoints.sort()
  return nPoints
}
/**
 * @param data
 * @param dataIn
 */
function getAttributes (data) {
  const attributes = []
  for (const key of Object.keys(data[0])) {
    if (typeof data[0][key] === 'object' &&
      !Array.isArray(data[0][key]) &&
      data[0] !== null) {
      attributes.push(key)
    }
  }
  return attributes
}
/**
 * @param data
 */
function getInitChoices (data) {
  const initChoices = []
  data.forEach(function (d) {
    if (initChoices.indexOf(d.INIT_CHOICE) === -1) {
      initChoices.push(d.INIT_CHOICE)
    }
  })
  initChoices.sort()
  return initChoices
}
/**
 * @param data
 */
function getProblems (data) {
  const problems = []
  data.forEach(function (d) {
    if (problems.indexOf(d.problem) === -1) {
      problems.push(d.problem)
    }
  }
  )
  problems.sort()
  return problems
}
/**
 * @param data
 */
function getNumClusters (data) {
  const numClusters = []
  data.forEach(function (d) {
    if (numClusters.indexOf(d.NUM_CLUST) === -1) {
      numClusters.push(d.NUM_CLUST)
    }
  })
  numClusters.sort()
  return numClusters
}
/**
 * @param data
 */
function groupedValues (data) {
  const curves = []
  getAttributes(data.grouped).forEach(function (attribute) {
    getSources(data.grouped).forEach(function (s) {
      const sData = data.grouped.filter(function (d) {
        return d.source === s
      })
      getNumClusters(sData).forEach(function (n) {
        getAlgorithms(sData).forEach(function (a) {
          const aData = sData.filter(function (d) {
            return d.IMPR_CLASS === a && d.NUM_CLUST === n
          })
          getInitChoices(sData).forEach(function (init) {
            const initData = aData.filter(x => x.INIT_CHOICE === init)
            getProblems(initData).forEach(function (problem) {
              const pbData = initData.filter(x => x.problem === problem)
              if (pbData.length === 0) {
                console.log(`No data for ${s} ${a} ${n} ${init} ${problem}`)
              }
              curves.push({
                type: 'line',
                with_markers: s === 'computed',
                points: pbData.map(function (d) {
                  return {
                    x: d.NUM_POINTS,
                    y: d[attribute].mean,
                    color: d.IMPR_CLASS
                  }
                }),
                metadata: {
                  source: s,
                  algorithm: a,
                  attribute: attribute,
                  type: 'simple',
                  minX: d3.min(pbData, function (d) { return d.NUM_POINTS }),
                  maxX: d3.max(pbData, function (d) { return d.NUM_POINTS }),
                  minY: d3.min(pbData, function (d) { return d[attribute].mean }),
                  maxY: d3.max(pbData, function (d) { return d[attribute].mean }),
                  init: init,
                  problem: problem,
                  numClusters: n
                }
              })
            })
          })
        })
      })
    })
  })
  glob.metadata = {
    algorithms: getAlgorithms(data.grouped),
    attributes: getAttributes(data.grouped)
  }
  curves.forEach(function (c) {
    c.colorValues = glob.metadata.algorithms
  })
  const num_clust = getNumClusters(data.grouped)
  num_clust.sort((a, b) => a - b)
  glob.ui.num_clust = num_clust
  glob.ui.num_clust_sel = -1
  glob.curves.push(...curves)
}
/**
 * @param data
 */
function groupedValuesDiff (data) {
  const curves = []
  getAttributes(data.grouped).forEach(function (attribute) {
    getSources(data.grouped).forEach(function (s) {
      const sData = data.grouped.filter(function (d) {
        return d.source === s
      })
      getNumClusters(sData).forEach(function (n) {
        getAlgorithms(sData).forEach(function (a) {
          const aData = sData.filter(function (d) {
            return d.IMPR_CLASS === a && d.NUM_CLUST === n
          })
          getAlgorithms(sData).forEach(function (aRef) {
            getInitChoices(sData).forEach(function (init) {
              getProblems(sData).forEach(function (problem) {
                const curveFilter = glob.curves.filter(
                  x =>
                    x.metadata.source === s &&
                    x.metadata.algorithm === aRef &&
                    x.metadata.attribute === attribute &&
                    x.type === 'line' &&
                    x.metadata.init === init &&
                    x.metadata.problem === problem &&
                    x.metadata.numClusters === n
                )
                if (curveFilter.length > 0) {
                  const newAData = aData.filter(x => x.INIT_CHOICE === init && x.problem === problem)
                  const curve = curveFilter[0]
                  curves.push({
                    type: 'line',
                    with_markers: s === 'computed',
                    points: newAData.map(function (d) {
                      const ref = curve.points.filter(x => x.x === d.NUM_POINTS)[0].y
                      var v = 0
                      if (ref !== 0) {
                        v = (d[attribute].mean - ref) / ref * 100
                      }

                      return {
                        x: d.NUM_POINTS,
                        y: v,
                        color: d.IMPR_CLASS
                      }
                    }),
                    metadata: {
                      source: s,
                      type: 'diff',
                      algorithm: a,
                      algorithmRef: aRef,
                      attribute: attribute,
                      minX: d3.min(newAData, function (d) { return d.NUM_POINTS }),
                      maxX: d3.max(newAData, function (d) { return d.NUM_POINTS }),
                      init: init,
                      problem: problem,
                      numClusters: n
                    }
                  })
                  curves[curves.length - 1].metadata.minY = d3.min(curves[curves.length - 1].points, function (d) { return d.y })
                  curves[curves.length - 1].metadata.maxY = d3.max(curves[curves.length - 1].points, function (d) { return d.y })
                }
              })
            })
          })
        })
      })
    })
  })
  curves.forEach(function (c) {
    c.colorValues = glob.metadata.algorithms
  })
  glob.curves.push(...curves)
}
/**
 * @param data
 */
function groupedValuesGreedyRandom (data) {
  const curves = []
  getAttributes(data.grouped).forEach(function (attribute) {
    getSources(data.grouped).forEach(function (s) {
      if (s === 'computed') {
        const sData = data.grouped.filter(function (d) {
          return d.source === s
        })
        getNumClusters(sData).forEach(function (n) {
          getProblems(sData).forEach(function (problem) {
            getAlgorithms(sData).forEach(function (a) {
              const aData = sData.filter(function (d) {
                return d.IMPR_CLASS === a && d.problem === problem && d.NUM_CLUST === n
              })
              if (aData.length > 0) {
                const curveInitPts = {}
                var nPts = []
                getInitChoices(sData).forEach(function (init) {
                  const initData = aData.filter(x => x.INIT_CHOICE === init)
                  const points = initData.map(function (d) {
                    return {
                      x: d.NUM_POINTS,
                      y: d[attribute].mean,
                      color: d.IMPR_CLASS
                    }
                  })
                  points.sort(d => d.x)
                  curveInitPts[init] = points

                  if (nPts.length === 0) {
                    nPts = points.map(x => x.x)
                  }
                })
                const points = []
                nPts.forEach(nPt => {
                  points.push({
                    x: nPt,
                    y:
                      curveInitPts.greedy.filter(x => x.x === nPt)[0].y -
                      curveInitPts.random.filter(x => x.x === nPt)[0].y,
                    color: curveInitPts.random.filter(x => x.x === nPt)[0].color
                  })
                })
                curves.push({
                  type: 'line',
                  with_markers: s === 'computed',
                  points: points,
                  metadata: {
                    source: s,
                    algorithm: a,
                    attribute: attribute,
                    type: 'simple',
                    minX: d3.min(points, function (d) { return d.x }),
                    maxX: d3.max(points, function (d) { return d.x }),
                    minY: d3.min(points, function (d) { return d.y }),
                    maxY: d3.max(points, function (d) { return d.y }),
                    init: '(greedy-random)',
                    problem: problem,
                    numClusters: n
                  }
                })
              }
            })
          })
        })
      }
    })
  })
  glob.metadata = {
    algorithms: getAlgorithms(data.grouped),
    attributes: getAttributes(data.grouped)
  }
  curves.forEach(function (c) {
    c.colorValues = glob.metadata.algorithms
  })
  glob.curves.push(...curves)
}
/**
 * @param data
 */
function groupedValuesGreedyRandomDiff (data) {
  const curves = []
  const curvesRef = glob.curves.filter(x => x.metadata.type === 'simple' && x.metadata.init === '(greedy-random)')
  getAttributes(data.grouped).forEach(function (attribute) {
    getProblems(data.grouped).forEach(function (problem) {
      getNumClusters(data.grouped).forEach(function (n) {
        getAlgorithms(data.grouped).forEach(function (a1) {
          getAlgorithms(data.grouped).forEach(function (a2) {
            var curve1 = curvesRef.filter(x =>
              x.metadata.algorithm === a1 &&
              x.metadata.attribute === attribute &&
              x.metadata.source === 'computed' &&
              x.metadata.problem === problem &&
              x.metadata.numClusters === n
            )
            var curve2 = curvesRef.filter(x =>
              x.metadata.algorithm === a2 &&
              x.metadata.attribute === attribute &&
              x.metadata.source === 'computed' &&
              x.metadata.problem === problem &&
              x.metadata.numClusters === n
            )
            if (curve1.length > 0 && curve2.length > 0) {
              curve1 = curve1[0]
              curve2 = curve2[0]
              const points = curve1.points.map(function (d) {
                return {
                  x: d.x,
                  y: (d.y - curve2.points.filter(x => x.x === d.x)[0].y) / curve2.points.filter(x => x.x === d.x)[0].y * 100,
                  color: d.color
                }
              })
              glob.curves.push({
                type: 'line',
                with_markers: true,
                points: points,
                metadata: {
                  source: 'computed',
                  type: 'diff',
                  algorithm: a1,
                  algorithmRef: a2,
                  attribute: attribute,
                  minX: d3.min(points, function (d) { return d.x }),
                  maxX: d3.max(points, function (d) { return d.x }),
                  minY: d3.min(points, function (d) { return d.y }),
                  maxY: d3.max(points, function (d) { return d.y }),
                  init: '(greedy-random)',
                  problem: problem,
                  numClusters: n
                }
              })
            }
          })
        })
      })
    })
  })
  curves.forEach(function (c) {
    c.colorValues = glob.metadata.algorithms
  })
  glob.curves.push(...curves)
}
/**
 * @param dataIn
 */
function filteredValues (dataIn) {
  const curves = []
  const data = dataIn.filtered
  const attributes = glob.metadata.attributes
  var extent = {}
  Object.keys(data).filter(x => x !== 'reference').forEach(function (s) {
    const sData = data[s]
    attributes.forEach(function (a) {
      const aData = sData
      const problem = aData.map(x => x.problem)[0]
      if (aData.length > 0) {
        const ext = d3.extent(aData.map(x => x[a]))
        const k = `${problem}-${a}`
        if (!extent[k]) {
          extent[k] = ext
        } else {
          extent[`${problem}-${a}`][0] = Math.min(extent[`${problem}-${a}`][0], ext[0])
          extent[`${problem}-${a}`][1] = Math.max(extent[`${problem}-${a}`][1], ext[1])
        }
      }
    })
  })
  Object.keys(data).filter(x => x !== 'reference').forEach(function (s) {
    const sData = data[s]
    attributes.forEach(function (a) {
      const aData = sData
      const pb = aData.map(x => x.problem)[0]
      const tmpXScale = d3.scaleLinear()
        .domain(extent[`${pb}-${a}`])// all keys
      const histogramCompute = d3.histogram()
        .value(d => d)
        .domain(tmpXScale.domain())
        .thresholds(tmpXScale.ticks(1000))
      const bins = histogramCompute(aData.map(x => x[a]))
      const minXLoc = d3.min(bins.filter(x => x.length > 0), function (d) { return d.x0 })
      const maxXLoc = d3.max(bins.filter(x => x.length > 0), function (d) { return d.x1 })
      curves.push({
        type: 'histogram',
        rectangles: bins.map(function (d) {
          return {
            x0: d.x0,
            x1: d.x1,
            count: d.length
          }
        }
        ),
        color: s.split('_')[1],
        metadata: {
          NUM_POINTS: aData[0].NUM_POINTS,
          attribute: a,
          source: aData[0].source,
          IMPR_CLASS: aData[0].IMPR_CLASS,
          minXLoc: minXLoc,
          maxXLoc: maxXLoc,
          minX: d3.min(bins, function (d) { return d.x0 }),
          maxX: d3.max(bins, function (d) { return d.x1 }),
          minY: d3.min(bins, function (d) { return d.length }),
          maxY: d3.max(bins, function (d) { return d.length }),
          init: aData[0].INIT_CHOICE,
          problem: pb,
          numClusters: aData[0].NUM_CLUST
        }
      })
    }
    )
  })
  console.log(curves)
  const colorValues = getAlgorithms(dataIn.grouped)
  curves.forEach(function (c) {
    c.colorValues = colorValues
  })
  glob.curves.push(...curves)
}
