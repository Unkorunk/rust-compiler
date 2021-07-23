# Rust compiler

### Description
This training project!

Compiler for the Rust written on C++. Tokenizing almost complete set of the Rust. Syntax analyzing a set described in comments in SyntaxParser.cpp. Semantic analyzing a almost complete subset of the implemented syntax (but not always correct in terms of Rust). Compiling a ultra small subset of the Rust (declaration variable with following types: i32, i64, f32 and f64. basic operations with this types, loop {}, while ... {}, if ... {} else if ... {} ... else {} and functions. there is a way for import external symbols using additional json file) into the WASM.

usage: `.\rust-compiler.exe .\main.rs`

input: `main.rs`, `main.json`

output: `index.html` and `index.wasm`

The `index.html` file contains the template for editing. See below for an example of the edited `index.html`.

example of `main.rs`:
```rust
fn calc_fib() -> i32 {
    let mut a = 1i32;
    let mut b = 1i32;
    let mut i = 2i32;

    while i != 22i32 {
        a += b;
        b = a - b;
        i += 1i32;
    }

    return a;
}

fn abs(x: f64) -> f64 {
    if x < 0f64 {
        return -x;
    }
    return x;
}

fn f(x: f64) -> f64 {
    return x * x;
}

fn int(mut from: f64, to: f64, frags: f64) -> f64 {
    let eps = 1e-6;

    let h = (to - from) / frags;
    let mut result = 0f64;
    while abs(from - to) >= eps {
        result += f(from) * h;
        from += h;
    }

    return result;
}

fn main() {
    print_f64(int(0f64, 2f64, 5000f64));
    print_i32(calc_fib());
}
```

example of `main.json`:
```json
{
  "imports": [
    {
      "module": "imports",
      "field": "print_f64",
      "type": {
        "params": [ "f64" ],
        "return": []
      },
      "associate": "print_f64"
    },
    {
      "module": "imports",
      "field": "print_i32",
      "type": {
        "params": [ "i32" ],
        "return": []
      },
      "associate": "print_i32"
    }
  ],
  "exports": [
    {
      "field": "exported_func",
      "type": {
        "params": [],
        "return": []
      },
      "associate": "main"
    }
  ]
}
```

example of `index.html`:
```html
<!doctype html>
<html>
  <body>
    <script>
      var importObject = {
          imports: {
              print_f64: (arg0) => { console.log(arg0); },
              print_i32: (arg0) => { console.log(arg0); },
          },
      };

      WebAssembly.instantiateStreaming(fetch('index.wasm'), importObject).then(obj => {
        obj.instance.exports.exported_func();
      });
    </script>
  </body>
</html>
```


### License
This repository is licensed with the [MIT](LICENSE) license.
