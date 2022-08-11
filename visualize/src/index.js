
import * as tsp from './scripts/tsp.js'
global.glob = {
  sizes: {
    width: window.innerWidth,
    height: window.innerHeight,
    svg: {
      margin_bottom: 40,
      margin_top: 5,
      margin_left: 40,
      margin_right: 40
    }
  },
  tooltip: {
    xoffset: 10,
    yoffset: 10
  },
  curves: [
    /*
    {
      type: 'line',
      with_markers: true,
      points: [{x:..., y:..., color:...}, {x:..., y:..., color:...}, ...]
      colorValues: ...
      dashed: ...
      metadata: ... // in order to identify the curve
    },
    {
      type: 'histogram',
      rectangles: [{x0:..., x1:..., count:...}, {x0:..., x1:..., count:...}, ...]
      color: ...
      metadata: ... // in order to identify the curve
    }
    */
  ],
  ui: {
    colorScale: null,
    algorithmsShown: [],
    histoValShown: new Set(),//{npoints:..., algorithm:...}
  }
};
(function (d3) {
  tsp.mainTSP(d3)
  window.addEventListener('resize', () => {
    tsp.resizeEvent(d3)
  })
})(d3)
