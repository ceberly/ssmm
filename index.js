// 10 megabytes of memory
var memory = new WebAssembly.Memory({initial:160, maximum:160});

// https://aransentin.github.io/cwasm/
function str_from_bytes(s, len) {
  let b = new Uint8Array(memory.buffer);
  let utf8decoder = new TextDecoder("utf-8");
  let a = b.subarray(s, s + len);

  return utf8decoder.decode(a);
}

var imports = {
  env: {
    memory: memory,
    say: function(str, len) { console.log(str_from_bytes(str, len)); },
    oops: function(str, len) {
      console.error(str_from_bytes(str, len));
      throw new Error("abort.");
    }
  }
};

WebAssembly.instantiateStreaming(fetch("ssmm.wasm"), imports)
.then(function(ssmm) {
  ssmm.instance.exports.memtest();

  console.log(memory);
  //ssmm.instance.exports.init_doom(1024 * 1024 * 10);

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
