<script lang="ts">
  import Clause from "./Clause.svelte";
  import type { MAXSATIn } from "../lib/types";

  export let result: MAXSATIn;
  export let currStep = 0;
  export let style = "";
  let clauses = result.clauses;
  let varModified = result.moves[currStep].var_id;
  // make a list where for each clause we have the last step where one of its variables was modified
  let lastModified = new Array(clauses.length).fill(-1);
  console.log(result,currStep);
  for (let i = 0; i <= currStep; i++) {
    let move = result.moves[i];
    for (let j = 0; j < clauses.length; j++) {
      let clause = clauses[j];
      let found = false;
      for (let k = 0; k < clause.length; k++) {
        if (clause[k][0] === move.var_id) {
          lastModified[j] = i;
          found = true;
          break;
        }
      }
    }
  }
  const clausesData = clauses.map((clause, i) => {
    const data =  clause.map((v) => {
        return {
          var: v[0],
          negation: v[1],
        };
      });
      data.sort((a, b) => {
        return a.var - b.var;
      });
    return {
      id: i,
      data:data,
      weight: result.clauses_weights[i],
      lastModified: lastModified[i],
    };
  });
  clausesData.sort((a, b) => {
    return b.lastModified - a.lastModified;
  });
</script>

<div>
  {#each clausesData as clause}
    <Clause clauseData={clause} clauseIndex={clause.id} {varModified} {style} />
  {/each}
</div>
