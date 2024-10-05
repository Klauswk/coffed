#ifndef PLUGIN_LOADER_H
#define PLUGIN_LOADER_H

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "formater.h"
#include "logger.h"

int load_plugin(Formater_Plugin* formater_plugin, const char* plugin_name);

#endif
