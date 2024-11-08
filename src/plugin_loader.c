#include "plugin_loader.h"

int load_plugin( Formater_Plugin* formater_plugin, const char* plugin_name) {
	
	if (formater_plugin == NULL) {
		log_info( "Provided formater structure is null, can't load the plugin\n");
		return 1;
	}

	char* plugin_callback_name;
	if (asprintf(&plugin_callback_name, "./formatters/%s.so", plugin_name) == -1) {
		log_info("Could not generate plugin lib path\n");
		return 1;
	}
	
	void* handle = dlopen(plugin_callback_name, RTLD_NOW);

	if (handle == NULL) {
		log_info("Error loading plugin %s, %s\n", plugin_name, dlerror());
	  free(plugin_callback_name);
		return 1;
	}	

	free(plugin_callback_name);
	
	if (asprintf(&plugin_callback_name, "%s_plugin_format", plugin_name) == -1) {
		log_info( "Could not generate the plugin format callback name");
		return 1;
	}

	formater_plugin->callback = (Format_Callback) dlsym(handle, plugin_callback_name);
	
	if (formater_plugin->callback == NULL) {
		log_info( "An error occour when loading the plugin function: %s\n", dlerror());
	  return 1;
	}
	
	free(plugin_callback_name);

	if (asprintf(&plugin_callback_name, "%s_plugin_name", plugin_name) == -1) {
		log_info( "Could not generate the plugin callback name");
		return 1;
	}
	
	formater_plugin->name_version_callback = (Format_Name_Version) dlsym(handle, plugin_callback_name);
  
  if (strcmp(plugin_name, formater_plugin->name_version_callback()) != 0) {
    log_info("The plugin file name and name_version do not match: file %s - plugin_name: %s\n", plugin_name, formater_plugin->name_version_callback);
    formater_plugin->name_version_callback = NULL;
    formater_plugin->callback = NULL;
    if (dlclose(handle) != 0) {
      log_info("An error happened when closing the DLL: %s\n", dlerror());
    }
    return 1;
  }

	if (formater_plugin->name_version_callback == NULL) {
		log_info( "An error occour when loading the plugin function: %s\n", dlerror());
		return 1;
	}

	free(plugin_callback_name);
	return 0;
}

