#include "plugin_loader.h"

int load_plugin( Formater_Plugin* formater_plugin, const char* plugin_name) {
	
	char* plugin_callback_name;
	if (asprintf(&plugin_callback_name, "./%s.so", plugin_name) == -1) {
		fprintf(stderr, "Couldnt generate plugin lib name\n");
		exit(1);
	}
	
	void* handle = dlopen(plugin_callback_name, RTLD_NOW);

	if (handle == NULL) {
		fprintf(stderr, "Error loading plugin %s, %s\n", plugin_name, dlerror());
		exit(1);
	}	

	free(plugin_callback_name);

	if (formater_plugin == NULL) {
		fprintf(stderr, "provided formater is null, cant load the plugin\n");
		exit(1);
	}
	
	if (asprintf(&plugin_callback_name, "%s_plugin_format", plugin_name) == -1) {
		fprintf(stderr, "Couldnt generate the plugin callback name");
		exit(1);
	}

	formater_plugin->callback = (Format_Callback) dlsym(handle, plugin_callback_name);
	
	if (formater_plugin->callback == NULL) {
		fprintf(stderr, "An error occour when loading the plugin function: %s\n", dlerror());
	exit(1);
	}
	
	free(plugin_callback_name);

	if (asprintf(&plugin_callback_name, "%s_plugin_name", plugin_name) == -1) {
		fprintf(stderr, "Couldnt generate the plugin callback name");
		exit(1);
	}
	
	formater_plugin->name_version_callback = (Format_Name_Version) dlsym(handle, plugin_callback_name);

	if (formater_plugin->name_version_callback == NULL) {
		fprintf(stderr, "An error occour when loading the plugin function: %s\n", dlerror());
		exit(1);
	}

	free(plugin_callback_name);
}

