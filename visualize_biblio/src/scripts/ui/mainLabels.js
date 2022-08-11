/**
 *
 */
var force
/**
 *
 */
export function mainUI () {
  d3.select('main')
    .style('overflow', 'scroll')
    .style('width', window.innerWidth + 'px')
    .style('height', (window.innerHeight / 2) + 'px')
    .style('resize', 'vertical')
  const svg = d3.select('main')
    .append('svg')
    .attr('viewBox', '0,0,' + (glob.sizes.width * 10) + ',' + (glob.sizes.height * 10))
    .style('width', (glob.sizes.width * 10) + 'px')
    .style('height', (glob.sizes.height * 10) + 'px')
    .append('g')
    .attr('transform', 'translate(' + glob.margins.left + ',' + glob.margins.top + ')')
    // Initialize the links
  svg.append('rect')
    .attr('x', 200)
    .attr('y', 0)
    .attr('width',400)
    .attr('height',50)
    .on('click', function () {
      refreshedLinked()
      clear()
      mainUI()
    })
  svg
    .selectAll('line')
    .data(glob.data.categories.links)
    .enter()
    .append('line')
    .attr('class', 'links')
    .style('stroke', '#aaa')
    .style('stroke-width', '3px')
    .on('mouseenter', function () {
      d3.select(this)
        .style('stroke', 'red')
    })
    .on('mouseleave', function (d) {
      if (glob.data.categories.links.filter(x => x.source === d.source && x.target === d.target).length > 0) {
        d3.select(this)
          .style('stroke', '#aaa')
      }
    })
    .on(
      'click', function (d) {
        if (glob.data.categories.links.filter(x => x.source === d.source && x.target === d.target).length > 0) {
          glob.data.categories.links = glob.data.categories.links.filter(x => x.source !== d.source || x.target !== d.target)
        } else {
          glob.data.categories.links.push(d)
        }
        d3.select(this)
          .style('stroke', 'red')
        // refreshedLinked()
        // clear()
        // mainUI()
      }
    )
  // Initialize the nodes
  const node = svg
    .selectAll('g')
    .data(glob.data.categories.nodes)
    .enter()
    .append('g')
    .attr('class', 'node')
    .attr('x', 0)
    .attr('y', 0)
    .attr('transform', 'translate(0,0)')
    .on('mouseenter', function (d) {
      d3.select('main')
        .append('div')
        .style('width', '500px')
        .attr('id', 'tooltip')
        .style('position', 'absolute')
        .style('z-index', '10')
        .style('background', 'white')
        .style('padding', '5px')
        .style('border-radius', '5px')
        .style('box-shadow', '1px 1px 5px black')
        .style('left', d3.event.pageX + glob.tooltip.xoffset + 'px')
        .style('top', d3.event.pageY + glob.tooltip.yoffset + 'px')
        .html(d.description)
    })
    .on('mouseleave', function (d) {
      d3.select('#tooltip')
        .remove()
    })
  node
    .append('rect')
    .attr('class', 'rect')
    .attr('width', glob.sizes.rect.default_width)
    .attr('height', glob.sizes.rect.text_size + glob.sizes.rect.margins.left + glob.sizes.rect.margins.right)
    .style('fill', '#69b3a2')
    .attr('x', 0)
    .attr('y', 0)
    .attr('rx', 5)
    .attr('ry', 5)
    .on('mouseenter', function () {
      d3.select(this)
        .style('opacity', '0.75')
    })
    .on('mouseleave', function () {
      d3.select(this)
        .style('opacity', '1')
    })
  node
    .append('text')
    .attr('class', 'txt')
    .text(d => d.text)
    .attr('text-anchor', 'middle')
    .attr('dominant-baseline', 'middle')
    .style('font-size', glob.sizes.rect.text_size)
    // Let's list the force we wanna apply on the network
  force = d3.forceSimulation(glob.data.categories.linked_nodes) // Force algorithm is applied to data.nodes
    .force('link', d3.forceLink() // This force provides links between nodes
      .id(function (d) { console.log(d);return d.id }) // This provide  the id of a node
      .links(glob.data.categories.links) // and this the list of links
    )
    .force('charge', d3.forceManyBody().strength(-4e-2 * Math.pow(Math.min(glob.sizes.width, glob.sizes.height) / 2, 2))) // This adds repulsion between nodes. Play with the -400 for the repulsion strength
    .force('center', d3.forceCenter(glob.sizes.width / 2, glob.sizes.height / 2)) // This force attracts nodes to the center of the svg area
    .on('end', ticked)
}
/**
 *
 */
function ticked () {
  for (let i = 0; i < 5; i++) {
    force.tick()
  }
  d3.selectAll('.links')
    .attr('x1', function (d) { return d.source.x })
    .attr('y1', function (d) { return d.source.y })
    .attr('x2', function (d) { return d.target.x })
    .attr('y2', function (d) { return d.target.y })

  d3.selectAll('.node')
    .filter(x => glob.data.categories.linked_nodes.map(e => e.id).includes(x.id))
    .attr('transform', function (d) {
      const width = d3.select(this).select('text').node().getBoundingClientRect().width
      const rectWidth = width + glob.sizes.rect.margins.left + glob.sizes.rect.margins.right
      return 'translate(' + (d.x - rectWidth / 2) + ',' + (d.y - (glob.sizes.rect.text_size + glob.sizes.rect.margins.top + glob.sizes.rect.margins.bottom) / 2) + ')'
    })
  d3.selectAll('.rect')
    .attr('width', function (d) {
      const width = d3.select(this).select(function () { return this.parentNode }).select('text').node().getBoundingClientRect().width
      return width + glob.sizes.rect.margins.left + glob.sizes.rect.margins.right
    })
  d3.selectAll('.txt').attr('transform', function (d) {
    const width = d3.select(this).node().getBoundingClientRect().width

    return `translate(${glob.sizes.rect.margins.left + width / 2},${glob.sizes.rect.margins.top + glob.sizes.rect.text_size / 2})`
  })
}
/**
 *
 */
export function clear () {
  d3.select('main').selectAll('*').remove()
}
/**
 *
 */
function refreshedLinked () {
  glob.data.categories.linked_nodes = glob.data.categories.nodes.filter(x => {
    return glob.data.categories.links.map(e => e.source).includes(x) || glob.data.categories.links.map(e => e.target).includes(x)
  })
}
