# Build and Integration

- Language: any C++23 compiler (Clang, GCC, MSVC); compiles to WASM via Emscripten with zero changes. No platform headers, GPU, or audio (`docs/plan.md` §6).
- Runtime dependencies: C++ standard library only, plus `nlohmann/json` `v3.11.3` via FetchContent for type serialization (`CMakeLists.txt`).
- Test dependency: `doctest` `v2.4.11` via FetchContent, gated by `EXT_CORE_BUILD_TESTS=ON` (`CMakeLists.txt`).
- Library target: static `exd-core`, alias `exd::core`; public include `include/`, private `src/` (`CMakeLists.txt`).
- Entry points: `./build.sh` (add `--release` for release), `./clean.sh`, `./test.sh`; manual `cmake -B build`, `cmake --build build -j`, `ctest --test-dir build` (`README.md`).
- Downstream integration via FetchContent (`GIT_REPOSITORY …/extropian-core.git`, `GIT_TAG main`) + `target_link_libraries(my_app PRIVATE exd::core)` (`README.md`).
- License: Business Source License 1.1, converts to Apache 2.0 on 2029-05-26 (`LICENSE`, `README.md`).

Do not record per-run test counts as durable facts; suite layout is `tests/{ecs,math,core}/` run through `ctest`.

Sources: `CMakeLists.txt`; `README.md` (Building, Dependencies, License); `docs/plan.md` §6.
