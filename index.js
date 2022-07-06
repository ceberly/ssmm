var imports = {
  env: {
    console_log: function(n) {
      console.log(n);
    }
  }
};

WebAssembly.instantiateStreaming(fetch("ssmm.wasm"), imports)
.then(function(ssmm) {
  ssmm.instance.exports.doom_init(memory.buffer, 1024 * 1024 * 10);

  ssmm.instance.exports.doom_tick(2, 100);

  console.log(ssmm.instance);

//  window.setInterval(function() {
//    var val = Math.floor(Math.random() * 100);
//    ssmm.instance.exports.tick_doom(2, val);
//
//    var b = new Uint8Array(memory.buffer);
//
//    console.log(val);
//    console.log(ssmm.instance.exports.tree_height());
//  }, 1000);
});
