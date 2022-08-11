
import * as preprocess from './tsp/preprocess.js'
var data
var attributes
var authorizedAlgorithms
var algorithms
var authorizedResults
var algoRef
/**
 * @param d3
 */
export function mainTSP (d3) {
  d3.json('./table1_visualize.json').then(function (dataIn) {
    data = dataIn

    preprocess.main(data)
    glob.ui.colorScale = d3.scaleOrdinal()
      .domain([...new Set(Array.from(glob.curves.filter(x => x.type === 'line').map(x => x.metadata.algorithm)))])
      .range(['#0066ff', '#ff0000', '#ff9900', '#00ff00', '#0000ff', '#ff00ff', '#00ffff', '#ffff00'])
    glob.ui.algorithmsShown = glob.ui.colorScale.domain()
    makeLayout(d3, data)
    createLegend(d3)
    resizeEvent(d3)
  })
}
/**
 * @param d3
 */
function createLegend (d3) {
  const legendElem = d3.select('#tsp-legend')
    .selectAll('div')
    .data(glob.metadata.algorithms)
    .enter()
    .append('div')
    .style('display', 'inline-block')
    .style('margin-left', 1 + '%')
    .style('cursor', 'pointer')
  legendElem
    .append('div')
    .style('width', '20px')
    .style('height', '3px')
    .style('border-top', d => '3px solid ' + glob.ui.colorScale(d))
    .style('display', 'inline-block')
    .style('margin-right', '10px')
  legendElem
    .append('div')
    .text(d => d)
    .style('display', 'inline-block')

  legendElem.on('click', function (d) {
    const algorithm = d
    const isShown = glob.ui.algorithmsShown.includes(algorithm)
    if (isShown) {
      glob.ui.algorithmsShown = glob.ui.algorithmsShown.filter(x => x !== algorithm)
      d3.select(this).style('opacity', '0.5')
    } else {
      glob.ui.algorithmsShown.push(algorithm)
      d3.select(this).style('opacity', '1.0')
    }
    resizeEvent(d3, false)
  })
}

/**
 * @param d3
 * @param data
 */
function makeLayout (d3, data) {
  const tspDiv = d3.select('main')
    .append('div')
    .attr('id', 'tsp-container')

  tspDiv.append('h1')
    .attr('id', 'problem-title')
    .text('TSP')
    .style('text-align', 'center')
    .style('font-family', 'sans-serif')
  tspDiv.append('h2')
    .text('Points in [0,100]x[0,100] square, unformly distributed')
    .style('font-family', 'sans-serif')
    .style('text-align', 'center')
  const choice = tspDiv.append('div')
    .attr('id', 'pb-choice')
  choice.append('div')
    .text('Problem:')
    .style('font-family', 'sans-serif')
    .style('display', 'inline-block')

  choice.append('select')
    .attr('id', 'pb-choice-select')
    .on('change', function () {
      glob.ui.num_clust_sel = -1
      d3.select('#problem-title').text(d3.select('#pb-choice-select option:checked').property('text').toUpperCase())
      resizeEvent(d3)
    })
    .selectAll('option')
    .data(['tsp', 'clustering'])
    .enter()
    .append('option')
    .attr('value', d => d)
    .text(d => d)
    .style('display', 'inline-block')
  const imprChoice = tspDiv.append('div')
    .attr('id', 'tsp-impr-choice')
  imprChoice.append('div')
    .text('Improvement:')
    .style('font-family', 'sans-serif')
    .style('margin-right', '10px')
    .style('display', 'inline-block')
  imprChoice.append('select')
    .attr('id', 'tsp-impr-choice-select')
    .on('change', function () {
      resizeEvent(d3)
    })
    .selectAll('option')
    .data(['random', 'greedy', '(greedy-random)'])
    .enter()
    .append('option')
    .attr('value', d => d)
    .text(d => d)
    .style('display', 'inline-block')

  tspDiv.append('div')
    .attr('id', 'tsp-legend')
    .style('text-align', 'center')
  for (const compile of ['', '-diff']) {
    for (const name of ['lines', 'histograms']) {
      const div = tspDiv.append('div')
        .attr('id', 'line-chart' + compile)
        .style('width', '100%')
        .style('height', '250px')
        .selectAll('.' + name)
        .data(glob.metadata.attributes)
        .enter()
        .append('div')
        .attr('class', name + compile)
        .style('width', (100 / glob.metadata.attributes.length - 2) + '%')
        .style('display', 'inline-block')
        .style('margin-left', 1 + '%')

      // title
      div.append('div')
        .attr('class', 'graph-title' + compile)
        .attr('id', d => 'title-line-' + d)
        .text(d => name === 'lines' ? d : '')
        .style('font-size', '20px')
        .style('font-weight', 'bold')
        .style('fill', 'black')
        .style('font-family', 'sans-serif')
        .style('text-anchor', 'middle')
        .style('text-align', 'center')
      // svg
      div
        .append('svg')
        .attr('width', '100%')
        .attr('height', '250px')
        .attr('id', d => name.slice(0, name.length - 1) + 'Chart' + compile + '-' + d)
        .style('margin-top', '0')
        .style('margin-bottom', '0')
    }
  }
  const controls = tspDiv.insert('div', '#line-chart-diff')
    .attr('id', 'tsp-diff-contols')
  controls.append('div')
    .text('Algorithm reference (algo-ref)/ref: ')
    .style('font-size', '14px')
    .style('font-weight', 'bold')
    .style('fill', 'black')
    .style('font-family', 'sans-serif')
    .style('display', 'inline-block')
    .style('margin-right', '10px')
  controls.append('select')
    .attr('id', 'tsp-diff-algo-ref')
    .style('display', 'inline-block')
    .on('change', function () {
      resizeEvent(d3)
    })
    .selectAll('option')
    .data(glob.metadata.algorithms)
    .enter()
    .append('option')
    .attr('value', d => d)
    .text(d => d)
}

/**
 * @param d3
 * @param data
 * @param temporary_still
 */
export function resizeEvent (d3, temporary_still = false) {
  const sizeLineChart = d3.select('svg').node().getBoundingClientRect()
  glob.sizes.width = sizeLineChart.width
  glob.sizes.height = sizeLineChart.height
  d3.selectAll('svg').selectAll('*').remove()
  d3.selectAll('.tooltip').remove()
  if (temporary_still == false) {
    d3.selectAll('.tmp').remove()
    additionalControls(d3)
  }
  drawLines(d3, '')
  drawHistograms(d3)
  drawLines(d3, '-diff')
}
/**
 * @param d3
 */
function additionalControls (d3) {
  if (d3.select('#pb-choice-select').property('value') === 'clustering') {
    const div = d3.select('#tsp-container')
      .insert('div', '#tsp-impr-choice')
      .attr('id', 'clustering-controls')
      .attr('class', 'tmp')
    div.append('div')
      .text('Number of clusters:')
      .style('font-family', 'sans-serif')
      .style('display', 'inline-block')
    const select = div.append('select')
      .attr('id', 'clustering-controls-select')
      .on('change', function (d) {
        glob.ui.num_clust_sel = parseInt(d3.select('#clustering-controls-select option:checked').property('text'))
        resizeEvent(d3, true)
      })
      .selectAll('option')
      .data(glob.ui.num_clust)
      .enter()
      .append('option')
      .attr('value', d => d)
      .text(d => d)
    select.property('value', glob.ui.num_clust_sel)
  }
}
/**
 * @param d3
 * @param identifier
 */
function drawLines (d3, identifier = '') {
  d3.selectAll('.lines' + identifier).each(function (d) {
    const type = (identifier === '-diff' ? 'diff' : 'simple')
    const dataOrig = glob.curves.filter(x => x.metadata.type === type)
    var data = dataOrig.filter(x => {
      return x.metadata.attribute === d &&
        x.type === 'line' &&
        glob.ui.algorithmsShown.includes(x.metadata.algorithm) &&
        x.metadata.init === d3.select('#tsp-impr-choice-select').property('value') &&
        x.metadata.problem === d3.select('#pb-choice-select').property('value') &&
        x.metadata.numClusters === glob.ui.num_clust_sel
    })
    if (identifier === '-diff') {
      data = data.filter(x => x.metadata.algorithmRef === d3.select('#tsp-diff-algo-ref').property('value'))
    }
    if (data.length === 0) {
      console.log('no data for ', d, identifier)
      return
    }
    // Scales
    const xScale = d3.scaleLinear()
      .domain([d3.min(data, x => x.metadata.minX), d3.max(data, x => x.metadata.maxX)])
      .range([0, glob.sizes.width - glob.sizes.svg.margin_left - glob.sizes.svg.margin_right])
    const yScale = d3.scaleLinear()
      .domain([d3.min(data, x => x.metadata.minY), d3.max(data, x => x.metadata.maxY)])
      .range([glob.sizes.height - glob.sizes.svg.margin_top - glob.sizes.svg.margin_bottom, 0])

    const colorScale = glob.ui.colorScale
    // Draw axes in groups
    const svg = d3.select(this).select('svg')
    svg.append('g')
      .attr('class', 'x-axis')
      .attr('transform', 'translate(' + glob.sizes.svg.margin_left + ',' + (glob.sizes.height - glob.sizes.svg.margin_bottom) + ')')
      .call(d3.axisBottom(xScale))
    svg.append('g')
      .attr('class', 'y-axis')
      .attr('transform', 'translate(' + glob.sizes.svg.margin_left + ',' + glob.sizes.svg.margin_top + ')')
      .call(d3.axisLeft(yScale))
    // Draw lines
    data.forEach(function (c) {
      const curve = svg.append('path')
        .attr('class', 'curve')
        .datum(c.points)
        .attr('d', d3.line()
          .x(function (e) { return xScale(e.x) })
          .y(function (e) { return yScale(e.y) })
        )
        .attr('stroke', colorScale(c.metadata.algorithm))
        .attr('stroke-width', '1')
        .attr('fill', 'none')
        .attr('transform', 'translate(' + glob.sizes.svg.margin_left + ',' + glob.sizes.svg.margin_top + ')')
      // Dotted line special case
      if (c.metadata.source === 'reference') { curve.attr('stroke-dasharray', '5,5') }
    })
    data.forEach(function (c) {
      // Scatter plot overlayed on line with more details
      if (c.with_markers && !(identifier === '-diff' && c.metadata.source === 'reference' && d3.select('#tsp-diff-algo-ref').property('value') === 'BICL')) {
        const elem = svg.append('g')
          .attr('class', 'markers')
          .selectAll('circle')
          .data(c.points)
          .enter()
          .append('circle')
          .attr('cx', function (e) { return xScale(e.x) })
          .attr('cy', function (e) { return yScale(e.y) })
          .attr('r', '2')
          .attr('fill', colorScale(c.metadata.algorithm))
          .attr('transform', 'translate(' + glob.sizes.svg.margin_left + ',' + glob.sizes.svg.margin_top + ')')
          .on('mouseenter', function (e) {
            d3.select(this).attr('r', '4')
            d3.select('main')
              .append('div')
              .attr('class', 'tooltip')
              .style('position', 'absolute')
              .style('z-index', '10')
              .style('background', 'white')
              .style('padding', '5px')
              .style('border-radius', '5px')
              .style('box-shadow', '1px 1px 5px black')
              .style('left', d3.event.pageX + glob.tooltip.xoffset + 'px')
              .style('top', d3.event.pageY + glob.tooltip.yoffset + 'px')
              .html('<b>N points:</b>' + e.x + '<br><b>' + c.metadata.attribute + '</b>:' + e.y.toFixed(2) + '<br><b>Algorithm:</b>' + c.metadata.algorithm)
            svg.append('path')
              .attr('class', 'vertiBar')
              .datum([{ x: e.x, y: yScale.domain()[0] }, { x: e.x, y: yScale.domain()[1] }])
              .attr('d', d3.line()
                .x(function (a) { return xScale(a.x) })
                .y(function (a) { return yScale(a.y) })
              )
              .attr('stroke', colorScale(c.metadata.algorithm))
              .attr('stroke-width', '2')
              .attr('fill', 'none')
              .attr('transform', 'translate(' + glob.sizes.svg.margin_left + ',' + glob.sizes.svg.margin_top + ')')
          })
          .on('mouseleave', function (e) {
            d3.select(this).attr('r', '2')
            d3.select('.tooltip').remove()
            svg.select('.vertiBar').remove()
          })
        if (identifier !== '-diff') {
          elem.on('click', function (e) {
            if (glob.ui.histoValShown.has(JSON.stringify([e.x, c.metadata.algorithm]))) {
              glob.ui.histoValShown.delete(JSON.stringify([e.x, c.metadata.algorithm]))
            } else {
              glob.ui.histoValShown.add(JSON.stringify([e.x, c.metadata.algorithm]))
            }
            resizeEvent(d3)
          })
        }
      }
    })
  })
}
/**
 * @param d3
 */
function drawHistograms (d3) {
  d3.selectAll('.histograms').each(function (histo) {
    const data = glob.curves.filter(
      x => {
        return glob.ui.histoValShown.has(JSON.stringify([x.metadata.NUM_POINTS, x.metadata.IMPR_CLASS])) &&
          x.type === 'histogram' &&
          x.metadata.source === 'computed' &&
          x.metadata.attribute === histo &&
          glob.ui.algorithmsShown.includes(x.metadata.IMPR_CLASS) &&
          x.metadata.init === d3.select('#tsp-impr-choice-select').property('value') &&
          x.metadata.problem === d3.select('#pb-choice-select').property('value') &&
          x.metadata.numClusters === glob.ui.num_clust_sel
      }
    )
    if (data.length === 0) { return }
    // Scales
    const xScale = d3.scaleLinear()
      .domain([d3.min(data, x => x.metadata.minXLoc), d3.max(data, x => x.metadata.maxXLoc)])
      .range([0, glob.sizes.width - glob.sizes.svg.margin_left - glob.sizes.svg.margin_right])
    const yScale = d3.scaleLinear()
      .domain([d3.min(data, x => x.metadata.minY), d3.max(data, x => x.metadata.maxY)])
      .range([glob.sizes.height - glob.sizes.svg.margin_top - glob.sizes.svg.margin_bottom, 0])
    const mapToColor = x => JSON.stringify([x.metadata.IMPR_CLASS, x.metadata.NUM_POINTS])
    const colorScale = d3.scaleOrdinal()
      .domain(data.map(x => mapToColor(x)))
      .range(d3.schemeCategory10)
    // Draw axes in groups
    const svg = d3.select(this).select('svg')
    svg.append('g')
      .attr('class', 'x-axis')
      .attr('transform', 'translate(' + glob.sizes.svg.margin_left + ',' + (glob.sizes.height - glob.sizes.svg.margin_bottom) + ')')
      .call(d3.axisBottom(xScale))
    svg.append('g')
      .attr('class', 'y-axis')
      .attr('transform', 'translate(' + glob.sizes.svg.margin_left + ',' + glob.sizes.svg.margin_top + ')')
      .call(d3.axisLeft(yScale))
    // Draw the rectangles
    data.forEach(function (c) {
      svg.append('g')
        .attr('class', 'single-histo')
        .attr('transform', 'translate(' + glob.sizes.svg.margin_left + ',' + glob.sizes.svg.margin_top + ')')
        .selectAll('rect')
        .data(c.rectangles)
        .enter()
        .append('rect')
        .attr('x', function (e) { return xScale(e.x0) })
        .attr('y', function (e) { return glob.sizes.height - glob.sizes.svg.margin_bottom - glob.sizes.svg.margin_top - (glob.sizes.height - glob.sizes.svg.margin_top - glob.sizes.svg.margin_bottom - yScale(e.count)) })
        .attr('width', function (e) { return Math.abs(xScale(e.x1) - xScale(e.x0)) })
        .attr('height', function (e) { return glob.sizes.height - glob.sizes.svg.margin_top - glob.sizes.svg.margin_bottom - yScale(e.count) })
        .attr('fill', function (e) { return colorScale(mapToColor(c)) })
        .attr('opacity', '0.5')
        .on('mouseenter', function (e) {
          d3.select(this).attr('opacity', '1.')
          d3.select('main')
            .append('div')
            .attr('class', 'tooltip')
            .style('position', 'absolute')
            .style('z-index', '10')
            .style('background', 'white')
            .style('padding', '5px')
            .style('border-radius', '5px')
            .style('box-shadow', '1px 1px 5px black')
            .style('left', d3.event.pageX + glob.tooltip.xoffset + 'px')
            .style('top', d3.event.pageY + glob.tooltip.yoffset + 'px')
            .html('<b>x0:</b>' + e.x0 + '<br><b>x1:</b>' + e.x1 + '<br><b>count:</b>' + e.count + '<br><b>Algo:</b>' + c.metadata.IMPR_CLASS + '<br><b>NumPoints:</b>' + c.metadata.NUM_POINTS)
        })
        .on('mouseleave', function (e) {
          d3.select(this).attr('opacity', '0.5')
          d3.select('.tooltip').remove()
        })
    })
  })
}
