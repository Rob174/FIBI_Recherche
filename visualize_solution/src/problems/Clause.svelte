<script lang="ts">
  import type { clause } from "../lib/types";

  export let clauseData: clause = { data: [], weight: 1 };
  export let clauseIndex: number = 0;
  export let varModified: number = -1;
  export let style = "";
  // Import mathjax
  import { onMount } from "svelte";
  import Katex from "./Katex.svelte";
  onMount(() => {
    let script = document.createElement("script");
    script.src =
      "https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.5/MathJax.js?config=TeX-MML-AM_CHTML";
    document.head.append(script);
  });
  let text = "clause \\text{ }";
  text += clauseIndex + "\\text{: weight = }";
  text += clauseData.weight.toFixed(2) + "\\text{: }";
  clauseData.data.forEach((v, i) => {
    text += v.negation ? "\\neg " : " ";
    if (varModified === v.var) {
      text += "\\color{red}";
    }
    text += "x_{" + v.var + "}";
    if (varModified === v.var) {
      text += "\\color{black}";
    }
    if (i !== clauseData.data.length - 1) {
      text += "\\lor";
    }
  });
  console.log(text);
</script>

<div>
  <Katex math={text} />
</div>

<style>
</style>
