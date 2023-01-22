<script lang="ts">
	import Range from "./Range.svelte"	

  export let minVal = 0;
  export let maxVal;
  export let onChange;
  export let style = "";
  let value = 0;
</script>

<div style={`transform: translateY(+50%);;${style};`}>
  <Range
    min={minVal}
    max={maxVal}
    initialValue={value}
    on:change={(v) => {
      value = v.detail.value;
      onChange(v.detail.value);
    }}
  />
</div>

<style>
  input {
    width: 5em;
    border: none;
  }
  input:focus {
    outline: none;
  }
  div {
    padding: 2em;
  }
</style>
