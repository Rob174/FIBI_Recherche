<script lang="ts">
  import Dialog, { Header, Title, Content, Actions } from "@smui/dialog";
  import Chip, { Set, LeadingIcon, TrailingIcon, Text } from "@smui/chips";
  import IconButton from "@smui/icon-button";
  import Button, { Label } from "@smui/button";
  import type { Paper } from "../../types";
  import { createEventDispatcher } from "svelte";
  const dispatch = createEventDispatcher();

  export let open = true;
  export let element: Paper = null;
  console.log(open,element);

  function closeHandler(e: CustomEvent<{ action: string }>) {
    switch (e.detail.action) {
      case "close":
        break;
    }
  }
</script>

<Dialog
  bind:open
  fullscreen
  aria-labelledby="fullscreen-title"
  aria-describedby="fullscreen-content"
  on:SMUIDialog:closed={closeHandler}
>
  {#if element}
    <Header>
      <Title id="fullscreen-title">{element.bibtex.title}</Title>
      <IconButton action="close" class="material-icons" on:click={() => dispatch("close")}>
        close
      </IconButton>
    </Header>
    <Content id="fullscreen-content">
      <div>
        Link: <a href={element.bibtex.URL}>{element.bibtex.URL}</a>
      </div>
      <div>
        Tags:
        <Set chips={element.tags} let:chip>
          <Chip {chip} shouldRemoveOnTrailingIconClick={false}>
            <Text>{chip}</Text>
          </Chip>
        </Set>
      </div>
      <div>Relevant texts</div>
      {#each element.relevant_text as t}
        <div>{t}</div>
      {/each}
      {#if element.analysis !== ""}
        <div>Analysis</div>
        <div>{element.analysis}</div>
      {/if}
    </Content>
    <Actions>
    </Actions>
  {/if}
</Dialog>
