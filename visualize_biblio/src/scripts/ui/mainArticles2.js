import * as preprocess from '../preprocessing/layoutGraph.js'
/* eslint-disable no-undef */
/**
  *
  */
export function mainUI () {
  d3.select('main')
    .style('width', sizes.articles.svg.width + 2 * sizes.articles.svg.padding + 'px')
    .style('height', sizes.articles.svg.height + 2 * sizes.articles.svg.padding + 'px')
  const canvas = d3.select('main')
    .append('svg')
    .attr('id', 'articlesTree')
    .attr('viewBox', `0,0,${sizes.articles.svg.width + 2 * sizes.articles.svg.padding},${sizes.articles.svg.height + 2 * sizes.articles.svg.padding}`)
    .style('width', sizes.articles.svg.width + 2 * sizes.articles.svg.padding)
    .style('height', sizes.articles.svg.height + 2 * sizes.articles.svg.padding)
    .append('g')
    .attr('transform', `translate(${sizes.articles.svg.padding},${sizes.articles.svg.padding})`)
  // insert all texts
  const nodes = canvas
    .selectAll('.node')
    .data(glob.data.articles.graph.nodes)
    .enter()
    .append('g')
    .attr('class', 'node')
    .on('mouseenter', function (d) {
      const node = glob.data.articles.graphLayout.nodes.filter(x => x.id === d.id)[0]
      if (node.isLeaf) {
        // Filter nodes ids with same names
        d3.selectAll('.node').nodes().forEach(function (a) {
          const selData = d3.select(a).data()[0]
          if (selData.name === d.name) {
            glob.ui.prevColors = d3.select(a)
              .select('rect')
              .attr('fill')
            d3.select(a)
              .select('rect')
              .attr('fill', 'var(--accent)')
          }
        })
      }
    })
    .on('mouseleave', function (d) {
      const node = glob.data.articles.graphLayout.nodes.filter(x => x.id === d.id)[0]
      if (node.isLeaf) {
        d3.selectAll('.node').nodes().forEach(function (a) {
          const selData = d3.select(a).data()[0]
          if (selData.name === d.name) {
            d3.select(a)
              .select('rect')
              .attr('fill', glob.ui.prevColors)
            glob.ui.prevColors = null
          }
        })
      }
    })
    .on('click', function (d) {
      const node = glob.data.articles.graphLayout.nodes.filter(x => x.id === d.id)[0]
      if (node.isLeaf) {
        createPopup(d, node)
      }
    })
  const textNum = nodes.append('text')
    .text(d => {
      const node = glob.data.articles.graphLayout.nodes.filter(x => x.id === d.id)[0]
      return (d.numUniqPapers > 0 && !node.isLeaf ? d.numUniqPapers + ' papers' : '')
    })
    .attr('dominant-baseline', 'hanging')
    .attr('text-anchor', 'start')
    .attr('fill', 'var(--front-without-bg)')
    .style('font-size', '12px')
    .style('pointer-events', 'none')
  const text = nodes.append('text')
    .text(d => d.name)
    .attr('dominant-baseline', 'hanging')
    .attr('text-anchor', 'start')
    .attr('fill', 'var(--front-with-bg)')
    // .style('pointer-events','none')
  const rect = nodes.append('rect')
    .attr('fill', 'var(--background)')
    .attr('rx', 5)
  // Update size of rectangles
  canvas.selectAll('.node').nodes().forEach(function (elem) {
    const d = d3.select(elem).data()[0]
    const index = glob.data.articles.graphLayout.nodes.indexOf(glob.data.articles.graphLayout.nodes.find(x => x.id === d.id))
    if (index !== -1) {
      glob.data.articles.graphLayout.nodes[index].width = elem.getBoundingClientRect().width
    } else {
      throw new Error('cannot find id in array')
    }
  })
  preprocess.maxWidthPerLevel()
  const maxWidthList = glob.data.articles.graphLayout.metadata.maxWidthPerLevel.map(x => x.maxWidth)
  const bandStep = [0]
  for (let i = 0; i < maxWidthList.length; i++) {
    bandStep.push(bandStep[i] + maxWidthList[i])
  }
  const scaleXinterm = d3.scaleOrdinal()
    .domain(glob.data.articles.graphLayout.metadata.maxWidthPerLevel.map(x => x.level))
    .range(bandStep)
  const scaleX = (level) => scaleXinterm(level) + (sizes.articles.svg.rect.paddingHor * 2 + sizes.articles.svg.horizSpace) * level
  const rectHeight = sizes.articles.svg.rect.height
  const scaleY = d3.scaleBand()
    .domain(Array.from(Array(glob.data.articles.graphLayout.metadata.totalLeaves).keys()).map(x => x + 1))
    .range([0, glob.data.articles.graphLayout.metadata.totalLeaves * rectHeight * 2])
    .padding(5)
  const xScaleRect = level => scaleX(level) - sizes.articles.svg.rect.paddingHor
  const yScaleRect = index => scaleY(index) - sizes.articles.svg.rect.paddingVert
  rect.attr('width', d => {
    const node = glob.data.articles.graphLayout.nodes.filter(x => x.id === d.id)[0]
    return node.width + sizes.articles.svg.rect.paddingHor * 2
  })
    .attr('height', sizes.articles.svg.rect.height)
    .attr('transform', d => {
      const node = glob.data.articles.graphLayout.nodes.filter(x => x.id === d.id)[0]
      var xtrans = xScaleRect(node.level)
      var ytrans = yScaleRect(node.index)
      return `translate(${xtrans},${ytrans})`
    })
  text.attr('transform', d => {
    const node = glob.data.articles.graphLayout.nodes.filter(x => x.id === d.id)[0]
    var xtrans = scaleX(node.level)
    var ytrans = scaleY(node.index)
    return `translate(${xtrans},${ytrans})`
  })
  textNum.attr('transform', d => {
    const node = glob.data.articles.graphLayout.nodes.filter(x => x.id === d.id)[0]
    var xtrans = scaleX(node.level) + 0
    var ytrans = scaleY(node.index) - sizes.articles.svg.rect.height * 3 / 4
    return `translate(${xtrans},${ytrans})`
  })
  textNum.raise()
  text.raise()
  var link = d3.linkHorizontal()
    .x(function (d) {
      const node = glob.data.articles.graphLayout.nodes.filter(x => x.id === d.id)[0]
      var x = xScaleRect(node.level)
      const offset = d.type === 'target' ? 0 : node.width + sizes.articles.svg.rect.paddingHor * 2
      return x + offset
    })
    .y(function (d) {
      const node = glob.data.articles.graphLayout.nodes.filter(x => x.id === d.id)[0]
      var y = yScaleRect(node.index) + sizes.articles.svg.rect.height / 2
      return y
    })
  canvas.selectAll('.link')
    .data(glob.data.articles.graph.links)
    .enter()
    .append('path')
    .attr('class', 'link')
    .attr('fill', 'none')
    .attr('stroke', 'var(--background)')
    .attr('d', d => {
      const dico = {}
      Array.from(['source', 'target']).forEach((out, i) => {
        dico[out] = { type: out, id: d[i] }
      })
      return link(dico)
    })
}

export function refreshSizes () {
  sizes.articles.svg.width = window.innerWidth * 100
  sizes.articles.svg.height = window.innerHeight * 100
}

function createPopup (d, node) {
  console.log(d)
  d3.select('main')
    .append('div')
    .attr('id', 'bg-popup')
    .attr('class', 'popup')
    .on('click', function () {
      d3.selectAll('.popup')
        .remove()
      d3.select('body').node().classList.remove('noscroll-class')
    })
  const popup = d3.select('main')
    .append('div')
    .attr('id', 'popup')
    .attr('class', 'popup')
  popup.append('h2')
    .text('Details')
  const popupContent = popup
    .append('div')
    .attr('id', 'popupContainer')
    .append('table')
  Array.from(['name', 'type', 'url', 'date', 'analysis']).forEach(name => {
    const r = popupContent.append('tr')
      .attr('id', name)
    r.append('td')
      .text(name.slice(0, 1).toUpperCase() + name.slice(1) + ':')
    const content = r.append('td')
    if (name in d && d[name] !== '') {
      content.text(d[name])
    } else {
      content.append('div')
        .style('color', 'red')
        .text('No data found')
    }
  })
  const url = popup.select('#url td:nth-child(2)')
    .text()
  popup.select('#url td:nth-child(2)').text('')
  popup.select('#url td:nth-child(2)')
    .append('a')
    .attr('href', url)
    .attr('target', '_blank')
    .text(url)
  const getNameTag = (name) => name === 'tags' ? 'name' : 'short_name'
  const getFunctionMoreInfos = (name) => name === 'tags' ? createDetailTagTag : createDetailTagProblem
  Array.from(['problem_tackled', 'tags']).forEach(name => {
    const r = popupContent.append('tr')
      .attr('id', name)
    r.append('td')
      .attr('class', 'tagName')
      .text(name.slice(0, 1).toUpperCase() + name.slice(1) + ':')
    const rectContainer = r.append('td')
    const divContainer = rectContainer.append('div')
    if (name in d) {
      const dicoList = d[name]
      const nameTag = getNameTag(name)
      dicoList.forEach(element => {
        let text = element[nameTag].slice(0, 10)
        if (element[nameTag].length > 9) {
          text += '...'
        }
        divContainer.append('div')
          .attr('class', 'tag')
          .text(text)
          .on('click', function () {
            if (!glob.ui.tagSelected) {
              const container = rectContainer.append('div').attr('class', 'tags-container')
              getFunctionMoreInfos(name)(container, element, glob.data.inputData, element[nameTag])
              glob.ui.tagSelected = true
            } else {
              d3.select('.tags-container').remove()
              glob.ui.tagSelected = false
            }
          })
      })
    } else {
      divContainer.append('div')
        .attr('class', 'tag')
        .text('No data')
    }
  })
  const name = 'extracted_informations'
  const r = popupContent.append('tr')
    .attr('id', name)
  r.append('td')
    .style('vertical-align', 'top')
    .text('Extracted texts: ')
  const list = r.append('td')
    .append('ol')
    .attr('start', 0)
  if (name in d) {
    d[name].forEach(textExtrDico => {
      list.append('li')
        .text(textExtrDico.content)
    })
    r.append('td')
    d3.select('body').node().classList.add('noscroll-class')
  } else {
    list.nodes().innerHtml = '<li style="color: red;">No data found</li>'
  }
  const rDone = popupContent.append('tr')
    .attr('id', 'done')
  rDone.append('td')
    .attr('class', 'tagName')
    .text('Done:')
  const rectContainer = rDone.append('td')
  const divContainer = rectContainer.append('div')
  divContainer.append('div')
    .attr('class', 'tag')
    .style('background-color', d.done ? 'var(--background)' : 'transparent')
    .style('border', 'solid 1px ' + (!d.done ? 'var(--background)' : 'transparent'))
    .text('Done')
    .on('click', function () {
      d.done = !d.done
      d3.select(this)
        .style('background-color', d.done ? 'var(--background)' : 'transparent')
        .style('border', 'solid 1px ' + (!d.done ? 'var(--background)' : 'transparent'))
      d3.selectAll('.node').nodes().forEach(function (a) {
        if (d3.select(a).data()[0].id === d.id) {
          const pastColor = d3.select(a)
            .select('rect')
            .attr('fill')
          d3.select(a)
            .select('rect')
            .attr('fill', pastColor === 'var(--done)' ? 'var(--background)' : 'var(--done)')
        }
      })
    })
}
function createDetailTagProblem (container, element, originalData, tagName) {
  container
    .text(element.description)
  container.on('click', function () {
    container.remove()
  })
}
function createDetailTagTag (container, element, originalData, tagName) {
  const table = container
    .append('table')
  const r1 = table.append('tr')
  r1.append('td').text('Tag description: ')
  r1.append('td').text(originalData.tags_description.filter(x => x.name === tagName)[0].description)
  const r2 = table.append('tr')
  r2.append('td').text('Additionnal infos: ')
  r2.append('td').text(JSON.stringify(element.additional_infos, undefined, 2))

  container.on('click', function () {
    container.remove()
  })
}
