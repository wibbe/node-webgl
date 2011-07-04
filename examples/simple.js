var webgl = require('../webgl');

console.log("Initializing...")
webgl.initialize();

var gl = webgl.getContext();

while (webgl.tick())
{
  gl.clear(gl.COLOR_BUFFER_BIT);
  
  gl.swapBuffers();
}


webgl.destroy();
console.log("Done!");