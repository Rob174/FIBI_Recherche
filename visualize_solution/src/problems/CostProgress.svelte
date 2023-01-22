<script lang="ts">
  import { onMount } from "svelte";

  export let costs: Array<{ x: number; y: number }> = [];
  let layout = {
    xaxis: {
      title: "Step",
    },
    yaxis: {
      title: "Cost",
    },
    height: "50vh"
  };
  onMount(() => {
    let script = document.createElement("script");
    script.src = "https://cdn.plot.ly/plotly-latest.min.js";
    document.head.append(script);
    console.log("costs", costs);
    script.onload = function () {
      Plotly.newPlot("cost-progress", [
        {
          x: costs.map((d,i) => i),
          y: costs.map((d) => d),
          color: "black",
          type: "scatter",
        },
      ], layout);
    };
  });
</script>
<div id="cost-progress">

</div>
<style>
  #cost-progress {
    width: 100%;
    height: 50vh;
  }
</style>