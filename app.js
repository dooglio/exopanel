const electron = require('electron')  // http://electron.atom.io/docs/api
const {app, BrowserWindow} = electron // http://electron.atom.io/docs/api
const path = require('path')          // https://nodejs.org/api/path.html
const url = require('url')            // https://nodejs.org/api/url.html

let window = null

// Wait until the app is ready
app.once('ready', () => {

  const {width, height } = electron.screen.getPrimaryDisplay().workAreaSize
  const win_width      = 400
  const win_height     = 500

  const win_x = (width/2) - win_width

  console.log( "width =" + width + ", height=" + height + ", win_x = " + win_x )

  // Create a new window
  window = new BrowserWindow({
    alwaysOnTop: true,
    frame: false,
    titleBarStyle: 'hidden',
    x: win_x,
    y: 0,
    // Set the initial width to 400px
    width: win_width,
    // Set the initial height to 500px
    height: win_height,
    // Don't show the window until it ready, this prevents any white flickering
    show: false,
    // Don't allow the window to be resized.
    resizable: false,

    backgroundColor: 'black',
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
    window.show()
  })
})
