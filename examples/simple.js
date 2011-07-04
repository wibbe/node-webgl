var webgl = require('../webgl');

console.log("Initializing...")
webgl.initialize();

var gl = webgl.getContext();

gl.clearColor(0.2, 0.4, 0.8, 1.0);

while (webgl.tick()) {
  gl.clear(gl.COLOR_BUFFER_BIT);
  gl.swapBuffers();
}

webgl.destroy();
console.log("Done!");