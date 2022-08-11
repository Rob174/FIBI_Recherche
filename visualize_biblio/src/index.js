import * as prepro from './scripts/preprocessing/mainPreprocessing.js'
import * as preproGraph from './scripts/preprocessing/graph.js'
import * as mainLabels from './scripts/ui/mainLabels.js'
import * as mainArticles from './scripts/ui/mainArticles2.js'
import * as graphLayout from './scripts/preprocessing/layoutGraph.js'
import * as theme from './scripts/ui/theme.js'
global.sizes = {
  innerHeight: null,
  innerWidth: null,
  articles: {
    svg: {
      width: null,
      height: null,
      padding: 50,
      rect: {
        height: 24,
        paddingHor: 5,
        paddingVert: 5
      },
      horizSpace: 50
    }
  }
}
global.glob = {
  sizes: {
    width: 1000,
    height: 1000,
    rect: {
      default_width: 100,
      text_size: 14,
      margins: {
        top: 10,
        bottom: 10,
        left: 10,
        right: 10
      }
    }
  },
  tooltip: {
    xoffset: 10,
    yoffset: 10
  },
  margins: {
    left: 150,
    right: 10,
    top: 10,
    bottom: 10
  },
  data: {
    categories: {
      links: [],
      nodes: []
    },
    articles: {
      graphLayout: {
        nodes: [],
        links: [],
        metadata: {}
      }
    }
  },
  ui: {
    theme: 'dark',
    tagSelected: false,
    prevColors:'black'
  }
};
(function (d3) {
  theme.addToogleThemeListener()
  Promise.all([d3.json('data.json'), d3.json('graph.json')])
    .then(function (datas) {
      const [data, graph] = datas
      glob.data.inputData = data
      mainArticles.refreshSizes()
      // prepro.main(data)
      // glob.data.categories.linked_nodes = glob.data.categories.nodes
      // mainLabels.mainUI()
      preproGraph.main(graph.graph, data)
      const nodes = glob.data.articles.graph.nodes.map(x => x.id)
      const links = glob.data.articles.graph.links
      const rootId = glob.data.articles.graph.nodes.filter(x => x.name === 'root')[0].id
      const [newGraph, newNodes, newLinks] = graphLayout.computeIndexes(nodes, links, rootId)
      glob.data.articles.graphLayout.nodes = newNodes
      glob.data.articles.graphLayout.links = newLinks
      build()
    })
  function build () {
    d3.select('main').selectAll('*').remove()
    mainArticles.refreshSizes()
    mainArticles.mainUI()
    theme.toogleTheme()
  }
  window.addEventListener('resize', () => {
    // build()
  })
})(d3)
