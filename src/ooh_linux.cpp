#include "tat.h"
#include "types.h"
#include "file_system.h"
#include "dynamic_libs.h"

#include "ooh.h"
#ifdef TAT_LINUX

#include "non_crypto_hash.h"
#include <dlfcn.h>
#include <string.h>

#include "log.h"

bool ooh::load_functions(uti::ptr dll_handle, const char* function_prefix, behaviour_functions* table)
{
	bool all_loaded = true;

	const int buffer_size = 256;
	char buffer[buffer_size];

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "load_all");
	table->load_all = (ooh_load_all_signature*)dlsym((void*)dll_handle, buffer);
	if (table->load_all == nullptr)
	{
		uti::log::inf_out("Unable to load function \"load_all\"");
		all_loaded = false;
	}

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "get_save_all_size");
	table->save_all_size = (ooh_get_save_all_size_signature*)dlsym((void*)dll_handle, buffer);
	if (table->save_all_size == nullptr)
	{
		uti::log::inf_out("Unable to load function \"get_save_all_size\"");
		all_loaded = false;
	}

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "save_all");
	table->save_all = (ooh_save_all_signature*)dlsym((void*)dll_handle, buffer);
	if (table->save_all == nullptr)
	{
		uti::log::inf_out("Unable to load function \"save_all\"");
		all_loaded = false;
	}

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "create");
	table->create = (ooh_create_signature*)dlsym((void*)dll_handle, buffer);
	if (table->create == nullptr)
	{
		uti::log::inf_out("Unable to load function \"create\"");
		all_loaded = false;
	}

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "load");
	table->load = (ooh_load_signature*)dlsym((void*)dll_handle, buffer);
	if (table->load == nullptr)
	{
		uti::log::inf_out("Unable to load function \"load\"");
		all_loaded = false;
	}

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "save");
	table->save = (ooh_save_signature*)dlsym((void*)dll_handle, buffer);
	if (table->save == nullptr)
	{
		uti::log::inf_out("Unable to load function \"save\"");
		all_loaded = false;
	}

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "data_size");
	table->data_size = (ooh_data_size_signature*)dlsym((void*)dll_handle, buffer);
	if (table->data_size == nullptr)
	{
		uti::log::inf_out("Unable to load function \"data_size\"");
		all_loaded = false;
	}

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "start");
	table->start = (ooh_start_signature*)dlsym((void*)dll_handle, buffer);
	if (table->start == nullptr)
	{
		uti::log::inf_out("Unable to load function \"start\"");
		all_loaded = false;
	}

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "update");
	table->update = (ooh_update_signature*)dlsym((void*)dll_handle, buffer);
	if (table->update == nullptr)
	{
		uti::log::inf_out("Unable to load function \"update\"");
		all_loaded = false;
	}

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "stop");
	table->stop = (ooh_stop_signature*)dlsym((void*)dll_handle, buffer);
	if (table->stop == nullptr)
	{
		uti::log::inf_out("Unable to load function \"stop\"");
		all_loaded = false;
	}

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "unload");
	table->unload = (ooh_unload_signature*)dlsym((void*)dll_handle, buffer);
	if (table->unload == nullptr)
	{
		uti::log::inf_out("Unable to load function \"unload\"");
		all_loaded = false;
	}

	return !all_loaded;
}

bool ooh::init_script(script_data* ooh_data)
{
	uti::u32 dll_name_len = (uti::u32)strnlen(ooh_data->dll_name, (size_t)script_data::max_dll_path);
	ooh_data->script_handle = uti::non_crypto_hash_32(&ooh_data->dll_name, dll_name_len);

	uti::ptr handle = uti::load_library(ooh_data->dll_name);

	// TODO: Force NULL/nullptr to 0 in load_library and similar?
	if(handle == 0)
	{
		uti::log::err_out("Unable to load ooh script \"%s\" (init_script)", ooh_data->dll_name);
		return false;
	}

	char buffer [script_data::max_dll_path] = {};
	if(!uti::get_path_library(handle, buffer, script_data::max_dll_path))
	{
		uti::unload_library(handle);
		uti::log::err_out("Unable to get loaded script path for \"%s\" (init_script)", ooh_data->dll_name);
		return false;
	}

	uti::unload_library(handle);

	memcpy(&ooh_data->dll_build_path[0], &buffer[0], script_data::max_dll_path);

	uti::u64 dll_path_len = strnlen(ooh_data->dll_build_path, script_data::max_dll_path);
	
	memcpy(ooh_data->dll_load_path, ooh_data->dll_build_path, dll_path_len);
	char* ext = &ooh_data->dll_load_path[dll_path_len - 5];
	const char* in_use_end = "_temp.ooh";
	memcpy(ext, in_use_end, strlen(in_use_end));

	memcpy(ooh_data->dll_func_prefix, ooh_data->dll_name, dll_name_len - 4);

	uti::update_file_mod_time(ooh_data->dll_build_path, &ooh_data->dll_build_update_time);

	return true;
}

// TODO: Just load the dll in init 
bool ooh::load_script(script_data* ooh_data)
{
	if(!uti::file_copy(ooh_data->dll_build_path, ooh_data->dll_load_path))
	{
		uti::log::inf_out("Unable to copy dll to load path (from %s to %s)", ooh_data->dll_build_path, ooh_data->dll_load_path);
		return false;
	}

	ooh_data->dll_handle = uti::load_library(ooh_data->dll_name);
	if(ooh_data->dll_handle == 0)
	{
		uti::log::inf_out("Unable to load dll %s", ooh_data->dll_name);
		return false;
	}

	uti::update_file_mod_time(ooh_data->dll_build_path, &ooh_data->dll_build_update_time);
	
	return load_functions(ooh_data->dll_handle, ooh_data->dll_func_prefix, &ooh_data->functions);
}

bool ooh::unload_script(script_data* ooh_data)
{
	uti::unload_library(ooh_data->dll_handle);
	ooh_data->dll_handle = 0;
	return true;
}

bool ooh::reload_script(script_data* ooh_data)
{
	if (!unload_script(ooh_data))
		return false;
	if (!load_script(ooh_data))
		return false;

	return true;
}

#endif
