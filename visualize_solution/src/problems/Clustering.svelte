<script lang="ts">
  import { clustersStatus } from "../lib/data";
  import type { ClusteringMove } from "../lib/types";
  import CostProgress from "./CostProgress.svelte";
  export let points: { x: number; y: number; cluster: number }[];
  export let labels: Array<string>;
  export let radius: number = 10;
  export let centroids: Array<{ x: number; y: number; color: string }>;
  export let style: string = "";
  export let animate: boolean = false;
  export let move: ClusteringMove;

  const minX = Math.min(...points.map((p) => p.x));
  const maxX = Math.max(...points.map((p) => p.x));
  const minY = Math.min(...points.map((p) => p.y));
  const maxY = Math.max(...points.map((p) => p.y));
  const scaleX = (x: number) => (x - minX) / (maxX - minX);
  const scaleY = (y: number) => (y - minY) / (maxY - minY);

  let pointLabelVisible = null;
  let clusterVisible = [];
  const availableColors = centroids.map((c) => c.color);
  let clusterVisibleSet = new Set();
  let clickedClusters = new Set();
  clustersStatus.subscribe((v) => {
    clusterVisibleSet = v.clusterVisibleSet;
    clickedClusters = v.clickedClusters;
    clusterVisible = Array.from(clusterVisibleSet);
  });

  type point = {
    x: number;
    y: number;
  };
  const dist = (p1: point, p2: point) => {
    return (p1.x - p2.x) ** 2 + (p1.y - p2.y) ** 2;
  };
  const clustDistToClust = new Map();
  points.forEach((p) => {
    if (!clustDistToClust.has(p.cluster)) {
      clustDistToClust.set(p.cluster, 0);
    }
    const centroid = centroids[p.cluster];
    clustDistToClust.set(
      p.cluster,
      clustDistToClust.get(p.cluster) + dist(p, centroid)
    );
  });
  const maxDist = Math.max(...Array.from(clustDistToClust.values()));
  export let clustersDistShown = false;
  export let pastCosts = [];
  export let showGraphCosts = false;
  let update = 0;
</script>

<div style="width: 100%; height:100%;">
  <!-- SVG filing the container -->
  {#if clustersDistShown}
    <div style="margin-left: 10%; margin-right:10%;">
      {#each centroids as centroid, k}
        <!-- Inline a bar with a text and the clustDistToClust corresponding to the cluster at the end -->
        <div>
          <div
            style={`width: ${
              (clustDistToClust.get(k) / maxDist) * 80
            }%; height: 1em; background-color: ${
              centroids[k].color
            } ; display: inline-block;`}
          />
          <div
            style="display: inline-block; margin-left: 1em; transform: translateY(-25%);"
          >
            {clustDistToClust.get(k)}
          </div>
        </div>
      {/each}
    </div>
  {/if}
  {#if showGraphCosts} 
    <CostProgress costs={pastCosts} />
  {/if}
  <svg
    style="width: 100%; height: 100%;"
    viewBox={`${-1920 / 8} ${-1080 / 8} ${1920 + 1920 / 4} ${1080 + 1080 / 4}`}
  >
    <defs>
      {#each availableColors as color}
        <pattern
          id={`diagonalHatch${color}`}
          width="6"
          height="6"
          patternUnits="userSpaceOnUse"
        >
          <path
            d="M-1,1 l2,-2 M0,4 l4,-4 M3,5 l2,-2"
            stroke={color}
            stroke-width="1"
          />
        </pattern>
      {/each}
    </defs>
    <!-- Creates one circle per point with radius specified -->
    {#each points as point, i}
      <!-- svelte-ignore a11y-click-events-have-key-events -->
      <!-- svelte-ignore a11y-mouse-events-have-key-events -->
      <!-- If the index i of the point corresponds to the move, add a bigger circle behind -->
      <!-- The r property of the circle loops between radius and radius * 2 -->
      {#if i==move.point_id}
        <circle
          cx={scaleX(point.x) * 1920}
          cy={scaleY(point.y) * 1080}
          r={radius * 2}
          fill="none"
          stroke="black"
          stroke-width="2"
        >
        <animate attributeName="stroke" begin="0s" dur="1s" repeatCount="indefinite" values={"black;red;black"}/>
        <animate attributeName="stroke-width" begin="0s" dur="1s" repeatCount="indefinite" values={"2;6;2"}/>
        </circle>
      {/if}
      <circle
      class="point"
        cx={scaleX(point.x) * 1920}
        cy={scaleY(point.y) * 1080}
        r={radius}
        fill={centroids[point.cluster].color}
        stroke="black"
        stroke-width="2"
        on:mouseover={() => {
          pointLabelVisible = i;
          if (!clickedClusters.has(point.cluster)) {
            console.log("add", point.cluster);
            clusterVisibleSet.add(point.cluster);
            clusterVisible = Array.from(clusterVisibleSet);
          }
          update++;
        }}
        on:mouseout={() => {
          pointLabelVisible = null;
          if (!clickedClusters.has(point.cluster)) {
            clusterVisibleSet.delete(point.cluster);
            clusterVisible = Array.from(clusterVisibleSet);
          }
          update++;
        }}
        on:click={() => {
          if (clickedClusters.has(point.cluster)) {
            clickedClusters.delete(point.cluster);
            clusterVisibleSet.delete(point.cluster);
            clusterVisible = Array.from(clusterVisibleSet);
          } else {
            clickedClusters.add(point.cluster);
            clusterVisibleSet.add(point.cluster);
            clusterVisible = Array.from(clusterVisibleSet);
          }
          update++;
        }}
      />
    {/each}
    <!-- For each point, create a dashed line between the point and the centroid affected  -->
    {#each points as point, i}
      {#key update}
        {#if clusterVisibleSet.has(point.cluster)}
          <line
            x1={scaleX(point.x) * 1920}
            y1={scaleY(point.y) * 1080}
            x2={scaleX(centroids[point.cluster].x) * 1920}
            y2={scaleY(centroids[point.cluster].y) * 1080}
            stroke={centroids[point.cluster].color}
            stroke-width="2"
            stroke-dasharray="5, 5"
            class="centroid-line"
          />
        {/if}
      {/key}
    {/each}
    <!-- Creates one circle with class centroid per centroid -->
    <!-- The centroid is filled with a dashed diagonal pattern of centroid.color -->
    {#each centroids as centroid, ic}
      {#key update}
        {#if clusterVisibleSet.has(ic)}
          <circle
            cx={scaleX(centroid.x) * 1920}
            cy={scaleY(centroid.y) * 1080}
            r={radius * 2}
            fill={`url(#diagonalHatch${centroid.color})`}
            stroke="black"
            stroke-width="2"
            class="centroid"
            style="pointer-events: none;"
          />
        {/if}
      {/key}
    {/each}
    <!-- Labels shown only when hover over a point -->
    {#each points as point, i}
      {#if pointLabelVisible === i}
        <!-- Div with the label -->
        <foreignObject
          x={scaleX(point.x) * 1920 + radius * 3}
          y={scaleY(point.y) * 1080 - radius * 2}
          width="10000"
          height="10000"
          stroke-width="0"
          style="pointer-events: none;"
        >
          <div class="tooltip">
            {`${labels[i]}`}
          </div>
        </foreignObject>
      {/if}
    {/each}
  </svg>
</div>

<style>
  /* Make the centroid-line moving in a loop */
  @-webkit-keyframes dash {
    to {
      stroke-dashoffset: 1000;
    }
  }

  .centroid-line {
    -webkit-animation: dash 30s linear infinite;
  }
  .tooltip {
    background-color: white;
    border: 1px solid black;
    padding: 0.5em;
    width: fit-content;
    border-radius: 0.5em;
    margin: 2px;
  }
  .point {
    cursor: pointer;
  }
</style>
