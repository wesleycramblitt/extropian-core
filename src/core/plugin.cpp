#include <dlfcn.h>
#include <string>
#include <cstdio>

namespace exd::core {

void* plugin_load(const std::string& path) {
    void* handle = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (!handle) {
        std::fprintf(stderr, "[plugin] Failed to load %s: %s\n",
                     path.c_str(), dlerror());
    }
    return handle;
}

void plugin_unload(void* handle) {
    if (handle) dlclose(handle);
}

void* plugin_get_symbol(void* handle, const std::string& name) {
    return dlsym(handle, name.c_str());
}

} // namespace exd::core
