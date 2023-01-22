<script lang="ts">
  import { onMount } from "svelte";
  import CostProgress from "./CostProgress.svelte";
  import type { point } from "../lib/types";
  export let points: point[];
  export let color: string = "black";
  export let labels: Array<string>;
  export let links: Array<{ source: number; target: number }> = [];
  export let tour: Array<number>;
  export let radius: number = 10;
  export let style: string = "";
  export let animate: boolean = false;
  const minX = Math.min(...points.map((p) => p.x));
  const maxX = Math.max(...points.map((p) => p.x));
  const minY = Math.min(...points.map((p) => p.y));
  const maxY = Math.max(...points.map((p) => p.y));
  const scaleX = (x: number) => (x - minX) / (maxX - minX);
  const scaleY = (y: number) => (y - minY) / (maxY - minY);
  let pointLabelVisible = null;
  let currPointPos = 0;
  const duration = 0.5;
  let visitedEdges = new Set();
  const correctionIntersecCircle = (
    src: { x: number; y: number },
    tgt: { x: number; y: number },
    radius: number
  ) => {
    const dx = tgt.x - src.x;
    const dy = tgt.y - src.y;
    const d = Math.sqrt(dx * dx + dy * dy);
    // Compute the coordinates of the intersections points between the line and the circle for the source and the target
    const srcIntersect = {
      x: src.x + (radius * dx) / d,
      y: src.y + (radius * dy) / d,
    };
    const tgtIntersect = {
      x: tgt.x - (radius * dx) / d,
      y: tgt.y - (radius * dy) / d,
    };

    return { source: srcIntersect, target: tgtIntersect };
  };
  let linksCorrected = links.map((link) => {
    const src = points[link.source];
    const tgt = points[link.target];
    return correctionIntersecCircle(src, tgt, (radius * 100) / 1080);
  });
  // In a loop make a second circle around the point visited and go to the next point in the points list
  // If the next point is the first last point in the list, then go back to the first point in the list
  onMount(() => {
    // Make an async function that will be called in an infinite loop
    async function updateProgressCircle() {
      // Get the circle element
      const progressCircle = document.getElementById("progressCircle");
      // Get the next point in the list
      const nextPoint = points[tour[currPointPos]];
      // If the next point is the last point in the list, then go back to the first point in the list
      if (currPointPos === 0) {
        visitedEdges = new Set();
        // Reset the stroke-color of all link-arrow to black
        links.forEach((link, index) => {
          const linkArrow = document.getElementById(`linkArrow${index}`);
          linkArrow.setAttribute("stroke", "black");
          // Reassign the arrow head to black
          linkArrow.setAttribute("marker-end", "url(#arrow)");
        });
      }
      if (currPointPos === points.length - 1) {
        currPointPos = 0;
      } else {
        currPointPos++;
      }
      // Make the position change with a progressive motion
      progressCircle.setAttribute("cx", scaleX(nextPoint.x) * 1920);
      progressCircle.setAttribute("cy", scaleY(nextPoint.y) * 1080);

      const cyclic_id = (p) => (p < 0 ? points.length + p : p);
      const idxLink = links.findIndex(
        (link) => link.target === tour[cyclic_id(currPointPos)]
      );
      console.log("idxLink", idxLink, currPointPos, links);
      visitedEdges.add(idxLink);
      // Set the stroke-color of all link-arrow of index in visitedEdges to red
      visitedEdges.forEach((index) => {
        console.log("index", index);
        if (index < 0) return;
        const linkArrow = document.getElementById(`linkArrow${index}`);
        linkArrow.setAttribute("stroke", "red");
        // Make arrow head red
        linkArrow.setAttribute("marker-end", "url(#arrowRed)");
      });
      // Wait for 1 second
      await new Promise((resolve) => setTimeout(resolve, duration * 2 * 1000));
      // Call the function again
      if (animate) {
        updateProgressCircle();
      }
    }
    // Call the function
    if (animate) {
      updateProgressCircle();
    }
  });
  export let pastCosts = [];
  export let showGraphCosts = false;
</script>

<div style="width: 100%; height:100%;">
  <!-- SVG filing the container -->
  {#if showGraphCosts} 
    <CostProgress costs={pastCosts} />
  {/if}
  <svg
    style="width: 100%; height: 100%;"
    viewBox={`${-1920 / 8} ${-1080 / 8} ${1920 + 1920 / 4} ${1080 + 1080 / 4}`}
  >
    <!-- Arrow marker -->
    <defs>
      <marker
        id="arrow"
        viewBox="0 0 10 10"
        refX="5"
        refY="5"
        markerWidth={radius / 2}
        markerHeight={radius / 2}
        orient="auto-start-reverse"
      >
        <!-- Arrow with curved end corners -->
        <path d="M 0 0 L 10 5 L 0 10 z" fill="black" />
      </marker>
      <!-- Red version of the arrow -->
      <marker
        id="arrowRed"
        viewBox="0 0 10 10"
        refX="5"
        refY="5"
        markerWidth={radius / 2}
        markerHeight={radius / 2}
        orient="auto-start-reverse"
      >
        <!-- Arrow with curved corners -->
        <path d="M 0 0 L 10 5 L 0 10 z" fill="red" />
      </marker>
    </defs>
    <!-- Make a circle with transparent fill and stroke blinking black red in infinite loop that have two times the radius of the points -->
    <!-- This circle position will change with a transition on the position -->
    {#if animate}
      <circle
        id="progressCircle"
        cx={scaleX(points[0].x) * 1920}
        cy={scaleY(points[0].y) * 1080}
        r={radius * 2}
        fill="transparent"
        stroke="black"
        stroke-width="2"
        style={`transition: cx ${duration}s, cy ${duration}s;`}
      />
    {/if}
    <!-- Points -->
    {#each points as point, i}
      <circle
        cx={scaleX(point.x) * 1920}
        cy={scaleY(point.y) * 1080}
        r={radius}
        fill={color}
        on:mouseover={() => (pointLabelVisible = i)}
        on:mouseout={() => (pointLabelVisible = null)}
      />
    {/each}
    <!-- Links as path stopping before reaching the circle -->
    {#each linksCorrected as link, i}
      <path
        id={`linkArrow${i}`}
        d={`M ${scaleX(link.source.x) * 1920} ${
          scaleY(link.source.y) * 1080
        } L ${scaleX(link.target.x) * 1920} ${scaleY(link.target.y) * 1080}`}
        stroke="black"
        stroke-width={radius / 2}
        fill="none"
        marker-end="url(#arrow)"
        style={`transition: stroke ${duration}s;`}
      />
    {/each}
    <!-- Labels shown only when hover over a point -->
    {#each points as point, i}
      {#if pointLabelVisible === i}
        <text
          x={scaleX(point.x) * 1920 + radius * 3}
          y={scaleY(point.y) * 1080 - radius * 2}
          fill="black"
          text-anchor="middle"
          dominant-baseline="middle"
          font-size="20"
          style="transition: opacity 2s;"
        >
          {labels[i]}
        </text>
      {/if}
    {/each}
  </svg>
</div>
