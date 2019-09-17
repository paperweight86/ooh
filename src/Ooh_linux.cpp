#include "tat.h"
#include "types.h"
#include "file_system.h"
#include "dynamic_libs.h"

#include "Ooh.h"
#ifdef TAT_LINUX

#include "non_crypto_hash.h"
#include <dlfcn.h>
#include <string.h>

#include "log.h"

bool Ooh::load_functions(uti::ptr dll_handle, const char* function_prefix, behaviour_functions* table)
{
	bool all_loaded = true;

	const int buffer_size = 256;
	char buffer[buffer_size];

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "load_all");
	table->load_all = (Ooh_load_all_signature*)dlsym((void*)dll_handle, buffer);
	if (table->load_all == nullptr)
	{
		uti::log::inf_out("Unable to load function \"load_all\"");
		all_loaded = false;
	}

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "get_save_all_size");
	table->save_all_size = (Ooh_get_save_all_size_signature*)dlsym((void*)dll_handle, buffer);
	if (table->save_all_size == nullptr)
	{
		uti::log::inf_out("Unable to load function \"get_save_all_size\"");
		all_loaded = false;
	}

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "save_all");
	table->save_all = (Ooh_save_all_signature*)dlsym((void*)dll_handle, buffer);
	if (table->save_all == nullptr)
	{
		uti::log::inf_out("Unable to load function \"save_all\"");
		all_loaded = false;
	}

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "create");
	table->create = (Ooh_create_signature*)dlsym((void*)dll_handle, buffer);
	if (table->create == nullptr)
	{
		uti::log::inf_out("Unable to load function \"create\"");
		all_loaded = false;
	}

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "load");
	table->load = (Ooh_load_signature*)dlsym((void*)dll_handle, buffer);
	if (table->load == nullptr)
	{
		uti::log::inf_out("Unable to load function \"load\"");
		all_loaded = false;
	}

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "save");
	table->save = (Ooh_save_signature*)dlsym((void*)dll_handle, buffer);
	if (table->save == nullptr)
	{
		uti::log::inf_out("Unable to load function \"save\"");
		all_loaded = false;
	}

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "data_size");
	table->data_size = (Ooh_data_size_signature*)dlsym((void*)dll_handle, buffer);
	if (table->data_size == nullptr)
	{
		uti::log::inf_out("Unable to load function \"data_size\"");
		all_loaded = false;
	}

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "start");
	table->start = (Ooh_start_signature*)dlsym((void*)dll_handle, buffer);
	if (table->start == nullptr)
	{
		uti::log::inf_out("Unable to load function \"start\"");
		all_loaded = false;
	}

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "update");
	table->update = (Ooh_update_signature*)dlsym((void*)dll_handle, buffer);
	if (table->update == nullptr)
	{
		uti::log::inf_out("Unable to load function \"update\"");
		all_loaded = false;
	}

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "stop");
	table->stop = (Ooh_stop_signature*)dlsym((void*)dll_handle, buffer);
	if (table->stop == nullptr)
	{
		uti::log::inf_out("Unable to load function \"stop\"");
		all_loaded = false;
	}

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "unload");
	table->unload = (Ooh_unload_signature*)dlsym((void*)dll_handle, buffer);
	if (table->unload == nullptr)
	{
		uti::log::inf_out("Unable to load function \"unload\"");
		all_loaded = false;
	}

	return !all_loaded;
}

bool Ooh::init_script(script_data* Ooh_data)
{
	uti::u32 dll_name_len = (uti::u32)strnlen(Ooh_data->dll_name, (size_t)script_data::max_dll_path);
	Ooh_data->script_handle = uti::non_crypto_hash_32(&Ooh_data->dll_name, dll_name_len);

	uti::ptr handle = uti::load_library(Ooh_data->dll_name);

	// TODO: Force NULL/nullptr to 0 in load_library and similar?
	if(handle == 0)
	{
		uti::log::err_out("Unable to load Ooh script \"%s\" (init_script)", Ooh_data->dll_name);
		return false;
	}

	char buffer [script_data::max_dll_path] = {};
	if(!uti::get_path_library(handle, buffer, script_data::max_dll_path))
	{
		uti::unload_library(handle);
		uti::log::err_out("Unable to get loaded script path for \"%s\" (init_script)", Ooh_data->dll_name);
		return false;
	}

	uti::unload_library(handle);

	memcpy(&Ooh_data->dll_build_path[0], &buffer[0], script_data::max_dll_path);

	uti::u64 dll_path_len = strnlen(Ooh_data->dll_build_path, script_data::max_dll_path);
	
	memcpy(Ooh_data->dll_load_path, Ooh_data->dll_build_path, dll_path_len);
	char* ext = &Ooh_data->dll_load_path[dll_path_len - 5];
	const char* in_use_end = "_temp.Ooh";
	memcpy(ext, in_use_end, strlen(in_use_end));

	memcpy(Ooh_data->dll_func_prefix, Ooh_data->dll_name, dll_name_len - 4);

	uti::update_file_mod_time(Ooh_data->dll_build_path, &Ooh_data->dll_build_update_time);

	return true;
}

// TODO: Just load the dll in init 
bool Ooh::load_script(script_data* Ooh_data)
{
	if(!uti::file_copy(Ooh_data->dll_build_path, Ooh_data->dll_load_path))
	{
		uti::log::inf_out("Unable to copy dll to load path (from %s to %s)", Ooh_data->dll_build_path, Ooh_data->dll_load_path);
		return false;
	}

	Ooh_data->dll_handle = uti::load_library(Ooh_data->dll_name);
	if(Ooh_data->dll_handle == 0)
	{
		uti::log::inf_out("Unable to load dll %s", Ooh_data->dll_name);
		return false;
	}

	uti::update_file_mod_time(Ooh_data->dll_build_path, &Ooh_data->dll_build_update_time);
	
	return load_functions(Ooh_data->dll_handle, Ooh_data->dll_func_prefix, &Ooh_data->functions);
}

bool Ooh::unload_script(script_data* Ooh_data)
{
	uti::unload_library(Ooh_data->dll_handle);
	Ooh_data->dll_handle = 0;
	return true;
}

bool Ooh::reload_script(script_data* Ooh_data)
{
	if (!unload_script(Ooh_data))
		return false;
	if (!load_script(Ooh_data))
		return false;

	return true;
}

#endif
