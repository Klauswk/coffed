#include "plugin_loader.h"

int load_plugin( Formater_Plugin* formater_plugin, const char* plugin_name) {
	
	char* plugin_callback_name;
	if (asprintf(&plugin_callback_name, "./formatters/%s.so", plugin_name) == -1) {
		log_info("Couldnt generate plugin lib name\n");
		return 1;
	}
	
	void* handle = dlopen(plugin_callback_name, RTLD_NOW);

	if (handle == NULL) {
		log_info("Error loading plugin %s, %s\n", plugin_name, dlerror());
		return 1;
	}	

	free(plugin_callback_name);

	if (formater_plugin == NULL) {
		log_info( "provided formater is null, cant load the plugin\n");
		return 1;
	}
	
	if (asprintf(&plugin_callback_name, "%s_plugin_format", plugin_name) == -1) {
		log_info( "Couldnt generate the plugin callback name");
		return 1;
	}

	formater_plugin->callback = (Format_Callback) dlsym(handle, plugin_callback_name);
	
	if (formater_plugin->callback == NULL) {
		log_info( "An error occour when loading the plugin function: %s\n", dlerror());
	return 1;
	}
	
	free(plugin_callback_name);

	if (asprintf(&plugin_callback_name, "%s_plugin_name", plugin_name) == -1) {
		log_info( "Couldnt generate the plugin callback name");
		return 1;
	}
	
	formater_plugin->name_version_callback = (Format_Name_Version) dlsym(handle, plugin_callback_name);

	if (formater_plugin->name_version_callback == NULL) {
		log_info( "An error occour when loading the plugin function: %s\n", dlerror());
		return 1;
	}

	free(plugin_callback_name);
	return 0;
}

