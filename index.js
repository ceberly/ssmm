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
  const imageData = ctx.createImageData(512, 512);

  for (let i = 0; i < imageData.data.length; i += 4) {
    // take the average of four contiguous bytes
    let buffer_sum = buffer[i] + buffer[i+1] + buffer[i+2] + buffer[i + 3];
    let buffer_avg = Math.round(buffer_sum / 4);

    // this is quite wasteful but whatever
    let prev_buffer_sum = prev_buffer[i] + prev_buffer[i+1] + prev_buffer[i+2] + prev_buffer[i + 3];
    let prev_buffer_avg = Math.round(prev_buffer_sum / 4);

    let new_red = buffer_avg == prev_buffer_avg ? imageData.data[i] - 1 : 255;
    let new_green = buffer_avg == prev_buffer_avg ? buffer_avg : 255;
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
  // this is constant in the C code, so really it does nothing here...
  var nbytes = 1024 * 1024; // 1MB

  var doom_buffer = ssmm.instance.exports.doom_buffer;
  var kandr_buffer = ssmm.instance.exports.kandr_buffer;

  ssmm.instance.exports.doom_init();

  var prev_doom_result = new Uint8ClampedArray(nbytes);
  var prev_kandr_result = new Uint8ClampedArray(nbytes);

  window.setInterval(function() {
    var chance = Math.floor(Math.random() * 2000000);
    var kandr_tree = ssmm.instance.exports.kandr_tick(chance);
    var kandr_result = new Uint8ClampedArray(
      ssmm.instance.exports.memory.buffer, kandr_buffer, nbytes);

    plot ('kandr-plot', kandr_result, prev_kandr_result);
    prev_kandr_result.set(kandr_result);
    
    var doom_tree = ssmm.instance.exports.doom_tick(chance);
    var doom_result = new Uint8ClampedArray(
      ssmm.instance.exports.memory.buffer, doom_buffer, nbytes);

    plot('doom-plot', doom_result, prev_doom_result);
    prev_doom_result.set(doom_result);

    // counts are the same in both trees, DRY out the tick() function
    // see comments in kandr.c
    var count = ssmm.instance.exports.tree_node_count(kandr_tree);
    var count_el = document.getElementById("tree-node-count");
    count_el.innerHTML = count;

  }, 10);
});
