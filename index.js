var imports = {
  env: {
    console_log: function(n) {
      console.log(n);
    }
  }
};

function plot(id, buffer, prev_buffer) {
  const canvas = document.getElementById(id);
  const ctx = canvas.getContext('2d');
  const imageData = ctx.createImageData(1024, 1024);

  for (let i = 0; i < imageData.data.length; i += 4) {
    let new_red = buffer[i] == prev_buffer[i] ? imageData.data[i] - 1 : 255;
    let new_green = buffer[i] == prev_buffer[i] ? buffer[i] : 255;
    let new_blue = 0;

    imageData.data[i + 0] = new_red;
    imageData.data[i + 1] = new_green;
    imageData.data[i + 2] = new_blue;
    imageData.data[i + 3] = 255;
  }

  ctx.putImageData(imageData, 0, 0);
}

WebAssembly.instantiateStreaming(fetch("ssmm.wasm"), imports)
.then(function(ssmm) {
  var nbytes = 1024 * 1024; // 1MB

  var buffer = ssmm.instance.exports.doom_buffer;

  ssmm.instance.exports.doom_init(buffer, nbytes);

  var prev_result = new Uint8ClampedArray(nbytes);
  window.setInterval(function() {
    var chance = Math.floor(Math.random() * 2000000);
    var tree = ssmm.instance.exports.doom_tick(chance);
    var result = new Uint8ClampedArray(
      ssmm.instance.exports.memory.buffer, buffer, nbytes);

    var count = ssmm.instance.exports.tree_node_count(tree);
    var count_el = document.getElementById("tree-node-count");
    count_el.innerHTML = count;

    plot('doom-plot', result, prev_result);

    prev_result.set(result);
  }, 10);
});
