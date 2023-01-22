<script>
    import Fa from 'svelte-fa'
    export let icon;
    export let color = "blue"
    export let onClick;
    export let fontSize = "";
    export let size = "";
    export let padding = "1em";
    export let style = "";
</script>

<!-- svelte-ignore a11y-click-events-have-key-events -->
<div class="button" style="--theme-color: {color}; color:var(--theme-color); font-size:{fontSize}; padding: {padding}; {style}" on:click={onClick}>
    <Fa icon={icon} size={size}/>
</div>

<style>
    .button {
        font-size: 1.5em;
        cursor: pointer;
        /** div takes only the text space */
        display: inline-block;
        aspect-ratio: 1/1;
        
    }
    .button:hover {
        /**Darken color with transition*/
        filter: brightness(0.8);
        /**Add transition*/
        transition: filter 0.2s;
    }
</style>