#include "../include/loader.h"
#include <dlfcn.h>
#include <stdio.h>

/**
 * @brief Linked list head for all loaded plugins.
 *
 * This global pointer stores the start of the plugin chain.
 * Each plugin is appended using add_plugin().
 */
plugin_t *plugin_list = NULL;

/**
 * @brief Add a plugin to the global plugin list.
 *
 * @param p Pointer to a plugin object initialized by init_plugin().
 *
 * This function prepends the new plugin to the global plugin_list.
 * If p is NULL, the function returns immediately.
 */
void add_plugin(plugin_t *p) {
    if (!p) return;
    p->next = plugin_list;
    plugin_list = p;
}

/**
 * @brief Dynamically load a plugin from a shared library.
 *
 * @param path Filesystem path to the .so plugin file.
 *
 * This function loads a plugin shared library using dlopen(),
 * resolves the init_plugin() entry symbol with dlsym(),
 * and initializes the plugin before adding it to plugin_list.
 *
 * Error handling:
 * - Prints an error if the .so cannot be opened.
 * - Prints an error if init_plugin is missing.
 * - Prints an error if init_plugin() returns NULL.
 *
 * Example:
 * @code
 * load_plugin("build/ext_file.so");
 * @endcode
 */
void load_plugin(const char *path) {
    void *handle = dlopen(path, RTLD_NOW);
    if (!handle) {
        fprintf(stderr, "Failed to load plugin %s: %s\n", path, dlerror());
        return;
    }

    plugin_t* (*init_plugin)(void) = dlsym(handle, "init_plugin");
    if (!init_plugin) {
        fprintf(stderr, "Failed to find init_plugin in %s: %s\n", path, dlerror());
        dlclose(handle);
        return;
    }

    plugin_t *p = init_plugin();
    if (!p) {
        fprintf(stderr, "Plugin %s returned NULL\n", path);
        dlclose(handle);
        return;
    }

    add_plugin(p);
}
