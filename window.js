// Run this function after the page has loaded
const {ipcRenderer} = require('electron');


function changeContents( id, contents ) {
    var idx = 0;
    $("#" + id).html( contents.long + " <small>(" + contents.short + ")</small>" )
    $("#" + id + "-price").text( contents.price );
    $("#" + id + "-change").text( contents.perc );
  }

function pollCoinCapIO() {
  const url = 'https://coincap.io/front';
  $.ajax(url).done((contents) => {
    //console.log(contents[0]);
    changeContents("one",   contents[0] )
    changeContents("two",   contents[1] )
    changeContents("three", contents[2] )
  }).fail((error) => {
    console.error(error)
  })
}

$(() => {
  console.log("start renderer")
  pollCoinCapIO();
  let timer = setInterval(() => {
    pollCoinCapIO()
  },10000)
})
