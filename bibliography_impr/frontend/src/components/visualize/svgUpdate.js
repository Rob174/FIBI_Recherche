import * as d3 from "d3";
export function updateSVG(papers, selectedNodeObj) {
  const svg = document.querySelector(".graph svg");
  // Check if svg exists, if not wait for it to exist
  if (!svg || !papers) {
    setTimeout(() => {
      updateSVG(papers,selectedNodeObj);
    }, 100);
    return;
  }
  // Set the size of the svg
  svg.setAttribute("width", "100%");
  svg.setAttribute("height", "100%");
  // Add drag and zoom with id graph0 for the main group containing everything
  const zoom = d3
    .zoom()
    .scaleExtent([0.1, 10])
    .on("zoom", function (event) {
      svg
        .querySelector("#graph0")
        .setAttribute("transform", event.transform.toString());
    });
  d3.select(svg).call(zoom);
  // get each .node title element text and find if it is in the list of tags or in the list of papers.bibtex.title[0]
  // if it is, then add a click event listener to it that will open the dialog
  d3.selectAll(".node title").each(function (d, i) {
    const title = d3.select(this).text();
    const node = d3.select(this).node();
    const parent = node.parentNode;
    const parentData = papers.find((p) => {
      return p.bibtex.title[0] === title;
    });
    if (parentData) {
      parent.style.cursor = "pointer";
      parent.addEventListener("click", () => {
        console.log("clicked", parentData);
        selectedNodeObj.selectedNode = parentData;
      });
      parent.addEventListener("mouseover", () => {
        // get the polygon inside parent and change the fill to red
        const polygon = parent.querySelector("polygon");
        polygon.setAttribute("stroke", "var(--accent2)");
        const text = parent.querySelector("text");
        text.setAttribute("fill", "var(--accent2)");
      });
      parent.addEventListener("mouseout", () => {
        // get the polygon inside parent and change the fill to red
        const polygon = parent.querySelector("polygon");
        polygon.setAttribute("stroke", "white");
        const text = parent.querySelector("text");
        text.setAttribute("fill", "white");
      });
    }
  });
  // Make the pointer be a hand when hovering over the svg
  d3.select("svg").style("cursor", "grab");
}
export async function requests() {
  // Make a fetch request to backend to get the papers
  let graph;
  const papers = await fetch(`http://localhost:3000/papers`)
    .then((response) => response.json())
    .then((data) => {
      console.log("updating21");
      return data;
    });
  await fetch(`http://localhost:3000/graph`)
    .then((response) => response.json())
    .then((data) => {
      graph = data.svg;
    })
    .catch((error) => {
      console.error("Error visualize:", error);
    });
    return {newPapers:papers, newGraph:graph}
}
