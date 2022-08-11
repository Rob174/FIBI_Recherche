/**
 * @param {string[]} nodes
 * @param {string[][]} links
 */
export function computeIndexes (nodes, links, rootId) {
  const graph = buildGraph(nodes, links, rootId)
  totalLeaves(graph)
  setTotalLeaves(graph, graph.leaves)
  glob.data.articles.graphLayout.metadata.totalLeaves = graph.leaves
  computeIndexForNodes(graph, 0, true)
  setLevelInfos(graph, 0)
  // console.log(graph)
  const newNodes = []
  const newLinks = []
  flatten(graph, newNodes, newLinks)
  // console.log(newNodes)
  return [graph, newNodes, newLinks]
}
function flatten (graph, nodes, links) {
  nodes.push({
    id: graph.id,
    childrenIds: graph.childrenIds,
    leaves: graph.leaves,
    totalLeaves: graph.totalLeaves,
    index: graph.index,
    level: graph.level,
    isLeaf: graph.isLeaf
  })
  graph.children.forEach(child => {
    links.push([graph.id, child.id])
    flatten(child, nodes, links)
  })
}
function setLevelInfos (graph, level) {
  graph.level = level
  graph.children.forEach(child => {
    setLevelInfos(child, level + 1)
  })
}
function buildGraph (nodes, links, currentNode) {
  const children = links.filter(x => x[0] === currentNode).map(x => {
    return x[1]
  })
  return {
    id: currentNode,
    childrenIds: children,
    children: children.map(x => buildGraph(nodes, links, x))
  }
}
function totalLeaves (graph) {
  if (graph.children.length === 0) {
    graph.leaves = 1
    graph.isLeaf = true
    return 1
  } else {
    var total = 0
    graph.children.forEach(x => {
      total += totalLeaves(x)
    })
    graph.leaves = total
    graph.isLeaf = false
    return total
  }
}
function setTotalLeaves (graph, total) {
  graph.totalLeaves = total
  if (graph.children.length !== 0) {
    graph.children.forEach(x => setTotalLeaves(x, total))
  }
}
function computeIndexForNodes (graph, offset, init = false) {
  // set an index in interval [0;totalLeaves)
  var currOffset = offset
  const middle = numChildren => numChildren / 2
  if (init) {
    graph.index = Math.ceil(middle(graph.leaves))
  }
  const offsetNext = (previousOffset, numChildrenPrev) => previousOffset + numChildrenPrev
  graph.children.forEach(child => {
    child.index = Math.ceil(currOffset + middle(child.leaves))
    computeIndexForNodes(child, currOffset)
    currOffset = offsetNext(currOffset, child.leaves)
  })
}

export function maxWidthPerLevel () {
  const levels = [...new Set(Array.from(glob.data.articles.graphLayout.nodes.map(x => x.level)))]
  glob.data.articles.graphLayout.metadata.maxWidthPerLevel = []
  levels.forEach(level => {
    const nodesLvl = glob.data.articles.graphLayout.nodes.filter(x => x.level === level)
    glob.data.articles.graphLayout.metadata.maxWidthPerLevel.push({
      level: level,
      maxWidth: d3.max(nodesLvl, x => x.width)
    })
  })
}

