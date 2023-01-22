<script lang="ts">
  import ShowConfig from "./ShowConfig.svelte";
  import { faAdd, faFileCirclePlus } from "@fortawesome/free-solid-svg-icons";
  import Animation from "./Animation.svelte";
  import { v4 as uuidv4 } from "uuid";
  import type {
    AbstractResult,
    ClusteringResult,
    ProblemResult,
    TSPResult,
  } from "./lib/types";
  import { clustersStatus, computeCosts } from "./lib/data";
  import IconButton from "./IconButton.svelte";
  import { onMount } from "svelte";
  import Clause from "./problems/Clause.svelte";
  let results: ProblemResult[] = [];
  let result: ProblemResult = null;
</script>

<main>
  <div class="action-bar">
    <IconButton
      icon={faAdd}
      color="white"
      onClick={() => {
        // Open json file, parse it and set the result
        document.getElementById("selectedFile1").click();
      }}
    />
    <!-- Add a dropdown with the uuids to select the problem -->
    <select
      on:change={(event) => {
        result = results.find((x) => x.id === event.target.value);
        clustersStatus.update((x) => {
          return {
            clusterVisibleSet: new Set(),
            clickedClusters: new Set(),
          };
        });
      }}
    >
      {#each results as result}
        <option value={result.id}>{result.id}</option>
      {/each}
    </select>
    {#if result !== null}
    <ShowConfig {result} classes="config" />
    {/if}
  </div>
  <div style="position: relative; width:100vw; height: fit-content;">
    {#key result}
      {#if result !== null}
        <Animation {result} />
      {/if}
    {/key}
  </div>
  <!-- <Clause clauseData={{data:[
    {var: 1, negation: false},
    {var: 2, negation: true},
    {var: 6, negation: false},
  ],weight: 5.23985}} clauseIndex={1} varModified={6} style="position: absolute; top: 25%;" /> -->
  <input
    type="file"
    id="selectedFile1"
    name="file"
    multiple
    style="display: none;"
    on:change={(event) => {
      const file = event.target.files[0];
      const reader = new FileReader();
      reader.onload = (event) => {
        const data = JSON.parse(event.target.result).map((x) => {
          return {
            ...x,
            id: uuidv4(),
          };
        });
        results = data;
        result = data[0];
        console.log("loading:", result);

        clustersStatus.update((x) => {
          return {
            clusterVisibleSet: new Set(),
            clickedClusters: new Set(),
          };
        });
      };
      reader.readAsText(file);
    }}
  />
</main>

<style>
  .action-bar {
    position: fixed;
    top: 0;
    left: 0;
    right: 0;
    height: 50px;
    background-color: #1e1e1e;
    display: flex;
    align-items: center;
    justify-content: flex-start;
    padding: 0 20px;
    z-index: 100;
  }
</style>
