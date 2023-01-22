<script lang="ts">
  import type { ProblemResult } from "./lib/types";


    export let result: ProblemResult;
    export let style = "";
    export let classes = "";
</script>
<div style={style} class={classes} id="config">
    {#each Object.entries(result.config).slice(0,5) as [key, value]}
        <p>{key}</p>
        <div class="config-elem">
            {#if typeof value === "string"}
                {value}
            {:else if typeof value === "number"}
                {value}
            {:else if typeof value === "boolean"}
                {value ? "True" : "False"}
            {/if}
        </div>
    {/each}
</div>
<style>
    p {
        font-weight: bold;
        color: white;
        display: inline-block;
    }
    .config-elem {
        display: inline-block;
        margin-right: 2em;
        margin-left: 1em;
        color: white;
    }
    #config {
        margin-left: 3em;
    }
</style>