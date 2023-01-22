<script lang="ts">
  import type { ProblemResult } from "./lib/types";
  import * as TSP from "./problems/TSP.svelte";
  import * as Clustering from "./problems/Clustering.svelte";
  import { getClusteringCost, getTSPCost, rainbow } from "./lib/utils";
  import Slider from "./Slider.svelte";
  import Maxsat from "./problems/MAXSAT.svelte";

  export let result: ProblemResult;
  let currStep = 0;
  let maxStep = 0;
  console.log(result);
  if (result.problem === "tsp") {
    maxStep = result.moves.length;
  } else if (result.problem === "clustering") {
    maxStep = result.moves.length;
  } else if (result.problem === "maxsat") {
    maxStep = result.moves.length;
  } else {
    maxStep = 0;
  }
  let animate = false;
  export let annotationsShown = true;
  let clustersDistShown = false;
</script>

<div style="width:100%;">
  <div style="background-color:white;position:sticky; top:0; z-index: 10;">
    <Slider
      style="width: 70%;display: inline-block; z-index: 0; margin-top: 1em;"
      minVal={0}
      maxVal={maxStep - 1}
      onChange={(newV) => {
        console.log(newV);
        currStep = Math.round(newV);
      }}
    />
    {#if result.problem === "tsp"}
      <div
        id="animate"
        class="push-button"
        on:click={() => {
          animate = !animate;
        }}
      >
        Animate
      </div>
    {/if}
    <div
      class="push-button"
      on:click={() => {
        annotationsShown = !annotationsShown;
      }}
    >
      Title
    </div>
    {#if result.problem === "clustering"}
      <div
        class="push-button"
        on:click={() => {
          clustersDistShown = !clustersDistShown;
        }}
      >
        Clusters distances
      </div>
    {/if}
    {#if annotationsShown}
      <h1 id="title">
        {"Step " +
          currStep +
          " / " +
          (maxStep - 1) +
          ": " +
          result.annotations[currStep]}
      </h1>
    {/if}
  </div>
  {#key currStep}
    {#if result.problem === "tsp"}
      {#key animate}
        <TSP.default
          points={result.towns.map((a) => ({ x: a[0], y: a[1] }))}
          labels={result.towns.map((a, i) => i.toString())}
          tour={result.tours[currStep]}
          links={result.tours[currStep].map((a, i) => {
            const tourLength = result.tours[currStep].length;
            return {
              source: a,
              target: result.tours[currStep][(i + 1) % tourLength],
            };
          })}
          {animate}
          pastCosts={result.quality.slice(0, currStep + 1)}
          showGraphCosts={true}
        />
      {/key}
    {:else if result.problem === "clustering"}
      {#key clustersDistShown}
        <Clustering.default
          points={result.points.map((a, i) => ({
            x: a[0],
            y: a[1],
            cluster: result.assignements[currStep][i],
          }))}
          labels={result.points.map((a, i) => `${i}: ${a}`)}
          centroids={result.centroids[currStep].map((a, k) => ({
            x: a[0],
            y: a[1],
            color: rainbow(result.centroids[0].length, k),
          }))}
          {clustersDistShown}
          pastCosts={result.quality.slice(0, currStep + 1)}
          showGraphCosts={true}
          move={result.moves[currStep]}
        />
      {/key}
    {:else if result.problem === "maxsat"}
      <Maxsat {result} currStep={currStep} />
    {/if}
  {/key}
</div>

<style>
  #animate:hover {
    background-color: #eee;
  }
  #title {
    /* Center  */
    font-size: 1em;
    display: flex;
    justify-content: center;
  }
  .push-button {
    border: 1px solid black;
    border-radius: 5px;
    padding: 0.5em;
    text-align: center;
    cursor: pointer;
    display: inline-block;
    width: fit-content;
  }
</style>
