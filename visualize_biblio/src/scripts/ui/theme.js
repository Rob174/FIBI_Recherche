export function addToogleThemeListener () {
  d3.select('#toogle-style')
    .on('click', function () {
      if (glob.ui.theme === 'light') {
        glob.ui.theme = 'dark'
      } else {
        glob.ui.theme = 'light'
      }
      toogleTheme()
    })
}
export function toogleTheme() {
  if (glob.ui.theme === 'light') {
    d3.select('body').node().classList.remove('dark-theme')
  } else {
    d3.select('body').node().classList.add('dark-theme')
  }
}
