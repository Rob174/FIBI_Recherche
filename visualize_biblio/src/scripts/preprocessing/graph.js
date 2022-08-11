import { v4 as uuidv4 } from 'uuid'
/* eslint-disable no-undef */
/**
 * @param graphIn
 * @param dataIn
 */
export function main (graphIn, dataIn) {
  glob.data.articles.hierarchy = build(graphIn, dataIn.articles)
  numPapers(glob.data.articles.hierarchy)
  glob.data.articles.graph = {
    nodes: [],
    links: []
  }
  buildGraph(glob.data.articles.hierarchy)
}
/**
 * @param parentGraph
 * @param childId
 */
function tagsOfSubgraph (parentGraph, childId) {
  // tag other is one time per hierarchy
  if (parentGraph.children[childId].tags === undefined) {
    return undefined
  }
  if (parentGraph.children[childId].tags.includes('others')) {
    if (parentGraph.children[childId].tags.length !== 1) {
      throw new Error('other is with an other tag. Must be alone')
    }
    const allOtherChildsTags = Array.from(parentGraph.children.filter((x, i) => i !== childId).map(x => x.tags).flat())
    return { excluded: allOtherChildsTags, included: [] }
  } else {
    return { included: parentGraph.children[childId].tags, excluded: [] }
  }
}
/**
 * @param currentTags
 * @param tagsRequ
 */
function tagCondOk (tagsArticle, tagsRequ) {
  if (tagsArticle.length === 0) {
    return false
  } else {
    if (tagsRequ.included.length > 0) {
      return tagsArticle.filter(x => (new Set(tagsRequ.included)).has(x)).length > 0 // has at least one of the tags required
    } else {
      return tagsArticle.filter(x => (new Set(tagsRequ.excluded)).has(x)).length === 0 // has none of the excluded tags
    }
  }
}
/**
 * @param graph
 * @param data
 * @param tags
 */
function build (graph, data, tags) {
  const out = {
    name: graph.name,
    tags: tags,
    children: [],
    _children: null,
    data: data
  }
  if (graph.children === undefined) {
    out.children = data
    return out
  }
  if (tags === undefined) {
    out.tags = []
    graph.children.forEach((subgraph, i) => {
      const dataForChildren = []
      const tagsPreprocessed = tagsOfSubgraph(graph, i)
      data.forEach(function (article, i) {
        const currentTags = article.tags.map(x => x.name)
        if (tagCondOk(currentTags, tagsPreprocessed) === true) {
          dataForChildren.push(article)
        }
      })
      out.children.push(build(subgraph, dataForChildren, tagsPreprocessed))
    })
  } else {
    graph.children.forEach((subgraph, i) => {
      const dataForChildren = []
      const tagsPreprocessed = tagsOfSubgraph(graph, i)
      data.forEach(function (article, i) {
        const currentTags = article.tags.map(x => x.name)
        if (tagCondOk(currentTags, tagsPreprocessed) === true) {
          dataForChildren.push(article)
        }
      })
      out.children.push(build(subgraph, dataForChildren, tagsPreprocessed))
    })
    out.tags = tags.included.length > 0 ? tags.included : ['others']
  }
  return out
}
function buildGraph (graphIn) {
  // nodes
  const graph = JSON.parse(JSON.stringify(graphIn))
  loopNodes(graph)
  function loopNodes (graph) {
    const graphCpy = graph
    graphCpy.id = uuidv4()
    if (graphCpy.children === undefined) {
      graphCpy.typeNode = 'article'
      glob.data.articles.graph.nodes.push(graphCpy)
    } else if (graphCpy.children.length === 0) {
      graphCpy.typeNode = 'category'
      glob.data.articles.graph.nodes.push(graphCpy)
    } else {
      graphCpy.typeNode = 'category'
      glob.data.articles.graph.nodes.push({
        name: graphCpy.name,
        tags: graphCpy.tags,
        id: graphCpy.id,
        children: graphCpy.children,
        data: graphCpy.data,
        numPapers: graph.numPapers,
        numUniqPapers: [...new Set(Array.from(graph.data.map(x => x.name)))].length,
        done: false
      })
      graph.children.forEach(child => {
        loopNodes(child)
      })
    }
  }
  function loopLinks (graph) {
    if (graph.children === undefined) {
      return
    }
    const currId = graph.id
    graph.children.forEach(function (subgraph) {
      const otherId = subgraph.id
      glob.data.articles.graph.links.push([currId, otherId])
      loopLinks(subgraph)
    })
  }
  loopLinks(graph)
}
function numPapers (graph) {
  if (graph.children === undefined) {
    graph.numPapers = 1
    return 1
  } else if (graph.children.length === 0) {
    graph.numPapers = 0
    return 0
  } else {
    var total = 0
    graph.children.forEach(x => {
      total += numPapers(x)
    })
    graph.numPapers = total
    return total
  }
}
