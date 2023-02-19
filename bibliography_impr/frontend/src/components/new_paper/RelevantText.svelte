<script lang="ts">
  import Textfield from "@smui/textfield";
  import IconButton from "@smui/icon-button";
  import { createEventDispatcher } from "svelte";
  export let text: string = "";
  export let className = "";
  const dispatch = createEventDispatcher();
  export let index: number;
  let numLines = 2;
  const handleInput = (event) => {
    const value = event.target.value;
    // Update the number of lines based on the input value
    numLines = (value.match(/\n/g) || []).length + 2;
  };
  export let label = "Relevant text";
  export let style="";
</script>

<div style={"width: 100%;"+style} class={className}>
  <!-- Make textfield adapt to text number of lines -->
  <Textfield
    rows={numLines}
    textarea
    bind:value={text}
    label={label}
    style={"width: 100%; margin-bottom: 2em;" + `height: ${numLines * 1.5}rem`}
    on:input={handleInput}
    
  />
  <IconButton
    class="material-icons"
    on:click={() => dispatch("remove", { index })}>delete</IconButton
  >
</div>

<style>
  div {
    display: flex;
    flex-direction: row;
    align-items: center;
    justify-content: center;
  }
</style>
