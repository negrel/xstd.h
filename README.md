# `xstd.h` - Extension of C standard library.

Extension of C standard library in pure C11 in a form of single-file header.

## Get started

To add `xstd` to your project, just copy and paste `xstd.h` which is a vendored
bundle of all header files.

### Getting latest bundle

You can get latest/unstable bundle by running `make bundle`

### Running the tests

```shell
make tests
```

## Conventions

### Globals

Globals are written in snake case and prefixed with `g_`.

### Type Names

Private type names are written in `snake_case` with an `xstd_` prefix.

Public type names are `typedef` with a `CamelCase` name.

### Interfaces

Interfaces implementations are decoupled in three parts:
* virtual table: a struct containing pointer to methods
* interface struct: a struct containing a pointer to a virtual table and an offset to the body
* body: private field specific to interface implementations

## Contributing

If you want to contribute to `xstd.h` to add a feature or improve the code contact
me at [negrel.dev@protonmail.com](mailto:negrel.dev@protonmail.com), open an
[issue](https://github.com/negrel/xstd.h/issues) or make a
[pull request](https://github.com/negrel/xstd.h/pulls).

## :stars: Show your support

Please give a :star: if this project helped you!

[![buy me a coffee](.github/images/bmc-button.png)](https://www.buymeacoffee.com/negrel)

## :scroll: License

MIT © [Alexandre Negrel](https://www.negrel.dev/)
