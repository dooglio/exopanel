const {app, BrowserWindow} = require('electron') // http://electron.atom.io/docs/api
const path = require('path')         // https://nodejs.org/api/path.html
const url = require('url')           // https://nodejs.org/api/url.html

let window = null

// Wait until the app is ready
app.once('ready', () => {
  // Create a new window
  window = new BrowserWindow({
    // Set the initial width to 400px
    width: 400,
    // Set the initial height to 500px
    height: 500,
    // Don't show the window until it ready, this prevents any white flickering
    show: false,
    // Don't allow the window to be resized.
    resizable: false,
  })

  // Load a URL in the window to the local index.html path
  window.loadURL(url.format({
    pathname: path.join(__dirname, 'index.html'),
    protocol: 'file:',
    slashes: true
  }))

  window.openDevTools()

  // Show window when page is ready
  window.once('ready-to-show', () => {
    console.log('showing window');
    /*
    const url = "https://api.coinmarketcap.com/v1/ticker/?limit=3";
    var request = require("request");
    request({
        url: url,
        json: true
      }, function( error, response, body ) {
      if( !error && response.statusCode === 200 ) {
          console.log(body);
        }
      }
    )
    */
    window.show()
  })
})
