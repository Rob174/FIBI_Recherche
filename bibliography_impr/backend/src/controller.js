const model = require("./model");
const request = require("request-promise");
const { spawn } = require('child_process');
/**
 *
 * @param {*} req body contains doi, relevant_text, tags. Url will be inferred from doi
 * @param {*} res
 */
const postPaper = async (req, res) => {
  console.log("postPaper with " + JSON.stringify(req.body, null, 2));
  const { doi, relevant_text, tags, analysis } = req.body;
  if (!doi || !relevant_text || !tags) {
    console.log("Missing parameters");
    res.status(400).send({ result: "Missing parameters" });
    return;
  }
  const bibtex = await getBibtex(doi);
  if (!bibtex) {
    console.log("Error: bibtex is null");
    res.status(400).send("Error: bibtex is null");
    return;
  }
  const paper = {
    id: doi,
    bibtex: bibtex,
    doi: doi,
    relevant_text: relevant_text,
    tags: tags,
    analysis: analysis || "",
  };
  const success = model.writePaper(paper);
  if (!success) {
    console.log("Error: paper already exists");
    res.status(400).send({ result: "Error: paper already exists" });
    return;
  }
  res.status(200).send({ result: "Added" });
};
const getBibtex = async (doi) => {
  const url = `https://api.crossref.org/works/${doi}`;
  const options = {
    uri: url,
    json: true,
  };
  const bibtex = await request(options);
  return bibtex.message;
};
const getPapers = (req, res) => {
  const papers = model.getPapers();
  res.status(200).send(papers);
};
const getTags = (req, res) => {
  const tags = model.getTags();
  res.status(200).send(tags);
};
const getGraph = (req, res) => {
  // Build the JSON: Paper as main node, tags as subnodes and papers.bitex.title[0] as final nodes
  const papers = model.getPapers();
  const tags = new Map();
  // console.log("Papers:");
  // console.log(papers);
  papers.forEach((paper) => {
    console.log(paper.bibtex.title[0]);
    paper.tags.forEach((tag) => {
      if (!tags.has(tag)) {
        tags.set(tag, []);
      }
      tags.get(tag).push(paper.bibtex.title[0]);
    });
  });
  console.log("Tags:");
  console.log(tags);
  // Build the JSON
  const graph = {
    digraph: { node: [], edge: [] },
  };
  // Add main node
  graph.digraph.node.push({
    id: "Paper",
    label: "Paper",
  });
  // Add papers
  papers.forEach((paper) => {
    graph.digraph.node.push({
      id: paper.bibtex.title[0],
      label: paper.bibtex.title[0],
    });
  });
  // Add tags
  tags.forEach((papers, tag) => {
    graph.digraph.node.push({
      id: tag,
      label: `{${papers.length} papers|${tag}}`,
    });
    graph.digraph.edge.push({
      from: 'Paper',
      to: tag,
    });
    papers.forEach((paper) => {
      graph.digraph.edge.push({
        from: tag,
        to: paper,
      });
    });
  });
  // Convert in dot
  let dot = "digraph G {rankdir=LR; bgcolor=transparent; \n";
  graph.digraph.node.forEach((node) => {
    dot += `"${node.id}" [label="${node.label}" shape=record  color=white fontcolor=white] \n`;
  });
  graph.digraph.edge.forEach((edge) => {
    dot += `"${edge.from}" -> "${edge.to}" [color=white fontcolor=white] \n`;
  });
  dot += "}";
  console.log(dot);
  // Render the graph with dot to svg
  const dotProcess = spawn("dot", ["-Tsvg"]);
  dotProcess.stdin.write(dot);
  dotProcess.stdin.end();
  let svg = "";
  dotProcess.stdout.on("data", (data) => {
    svg += data;
  }
  );
  dotProcess.stdout.on("end", () => {
    res.status(200).send({svg:svg});
  }
  );
  
};
const clean = (req, res) => {
  model.clean();
  console.log("Cleaned");
  res.status(200).send({ result: "Success" });
};
module.exports = {
  postPaper,
  getPapers,
  getTags,
  getGraph,
  clean,
};
