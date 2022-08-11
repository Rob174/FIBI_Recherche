export function createPopup(e,content: string) {
    cleanPopup(e)
    const div = document.createElement('div')
    div.setAttribute('id','popup')
    div.style.left = `${e.pageX+10}px`
    div.style.top = `${e.pageY+10}px`
    div.innerHTML += content
    document.body.appendChild(div)
}
export function cleanPopup(e) {
    document.querySelectorAll('#popup').forEach(x=>
        {
            x.remove()
        }
        )
}