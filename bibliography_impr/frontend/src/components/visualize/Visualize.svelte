<script lang="ts">
  import { onMount } from "svelte";
  import DialogDetail from "./DialogDetail.svelte";
  import CircularProgress from "@smui/circular-progress";
  import { updateSVG, requests } from "./svgUpdate";
  // Make a fetch request to backend to get the papers every 5 seconds
  let interval = 5000;
  let graph;
  let selectedNodeObj = {selectedNode: null};
  let papers;
  let updating = false;
  async function update() {
      updating = true;
      const {newPapers, newGraph} = await requests();
      papers = newPapers;
      graph = newGraph;
      updateSVG(newPapers, selectedNodeObj);
      updating = false;
  }
  onMount(async () => {
    updating = true;
    await update();
    setInterval(update, interval);
  });
</script>

<div class="container-visu">
  <div class="graph">
    {#if graph}
      {@html graph}
    {/if}
  </div>
</div>
{#if selectedNodeObj.selectedNode}
  {#key selectedNodeObj.selectedNode.id}
    <DialogDetail
      element={selectedNodeObj.selectedNode}
      on:close={() => (selectedNodeObj.selectedNode = null)}
      open={true}
    />
  {/key}
{/if}
{#if updating}
  <div
    style="display: flex; justify-content: center; position: absolute; top: 4em; right: 0em; transform: translate(-100%,0%);"
  >
    <CircularProgress style="height: 50px; width: 50px;" indeterminate />
  </div>
{/if}

<style>
  /* Make it so the graph takes all the remaining space of the container */
  .container-visu {
    display: flex;
    flex-direction: column;
    height: 94vh;
    width: 99.5vw;
    margin: 0;
  }
  .graph {
    flex: 1;
    display: flex;
    justify-content: center;
    align-items: center;
  }
</style>
