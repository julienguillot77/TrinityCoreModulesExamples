# TrinityCore module examples

Standalone stable C ABI v1 examples for the TrinityCore Module SDK.

## Included modules

- `mod-greeting` registers `trinity.example.greeting.v1`, reads module configuration, and emits a world-startup greeting.
- `mod-example` consumes the greeting service, registers a world callback, reads configuration, and registers `.module example hello`.

Both modules include only the installed `TrinityModule.h` SDK header. They do not include private TrinityCore headers or link core implementation targets.

## Build

Install the standalone [TrinityCoreModuleSDK](https://github.com/TrinityCore/TrinityCoreModuleSDK) package, then configure and build from this repository root:

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/trinitycore-module-sdk
cmake --build build --config RelWithDebInfo --parallel
```

For Visual Studio, set the generator and architecture during configure:

```powershell
cmake -S . -B build -DCMAKE_PREFIX_PATH=C:/sdk/TrinityCore `
  -G "Visual Studio 18 2026" -A x64
cmake --build build --config RelWithDebInfo --parallel
```

The output is under `build/bin/<configuration>/modules/`.

## Signing and deployment

Example manifests are unsigned until the release step. Do not deploy their placeholder signature values. Sign each final module package with the TrinityCore SDK `sign_module.py` tool, install the matching `<key_id>.pub` file under the server's `keys/` directory, and copy the complete signed module directory under the server's `modules/` directory.

## Complete guide

See [DEVELOPMENT.md](https://github.com/julienguillot77/TrinityCoreModuleSDK/blob/master/DEVELOPMENT.md) for the complete create → build → sign → deploy → database → runtime-test → release workflow.
