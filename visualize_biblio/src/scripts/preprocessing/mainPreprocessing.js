/**
 * @param data
 */
export function main (data) {
  console.log(data)
  const tags = data.tags_description.map((x, i) => {
    return {
      id: 1 + i,
      text: x.name,
      description: x.description
    }
  })
  glob.data.categories.links = tags.map(x => {
    return {
      source: 0,
      target: x.id,
    }
  })
  tags.push({
    id: 0,
    text: 'root',
    description: ''
  })
  glob.data.categories.nodes = tags
  console.log(glob.data.categories)
}
