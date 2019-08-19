#include "tat.h"
#include "types.h"
#include "file_system.h"

#include "ooh.h"
#ifdef TAT_LINUX

#include "non_crypto_hash.h"
#include <dlfcn.h>
#include <string.h>

bool ooh::load_functions(uti::ptr dll_handle, const char* function_prefix, behaviour_functions* table)
{
	bool all_loaded = true;

	const int buffer_size = 256;
	char buffer[buffer_size];

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "load_all");
	table->load_all = (ooh_load_all_signature*)dlsym((void*)dll_handle, buffer);
	if (table->load_all == nullptr)
		all_loaded = false;

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "get_save_all_size");
	table->save_all_size = (ooh_get_save_all_size_signature*)dlsym((void*)dll_handle, buffer);
	if (table->save_all_size == nullptr)
		all_loaded = false;

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "save_all");
	table->save_all = (ooh_save_all_signature*)dlsym((void*)dll_handle, buffer);
	if (table->save_all == nullptr)
		all_loaded = false;

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "create");
	table->create = (ooh_create_signature*)dlsym((void*)dll_handle, buffer);
	if (table->create == nullptr)
		all_loaded = false;

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "load");
	table->load = (ooh_load_signature*)dlsym((void*)dll_handle, buffer);
	if (table->load == nullptr)
		all_loaded = false;

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "save");
	table->save = (ooh_save_signature*)dlsym((void*)dll_handle, buffer);
	if (table->save == nullptr)
		all_loaded = false;

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "data_size");
	table->data_size = (ooh_data_size_signature*)dlsym((void*)dll_handle, buffer);
	if (table->data_size == nullptr)
		all_loaded = false;

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "start");
	table->start = (ooh_start_signature*)dlsym((void*)dll_handle, buffer);
	if (table->start == nullptr)
		all_loaded = false;

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "update");
	table->update = (ooh_update_signature*)dlsym((void*)dll_handle, buffer);
	if (table->update == nullptr)
		all_loaded = false;

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "stop");
	table->stop = (ooh_stop_signature*)dlsym((void*)dll_handle, buffer);
	if (table->stop == nullptr)
		all_loaded = false;

	snprintf(buffer, buffer_size, "%s_%s", function_prefix, "unload");
	table->unload = (ooh_unload_signature*)dlsym((void*)dll_handle, buffer);
	if (table->unload == nullptr)
		all_loaded = false;

	return !all_loaded;
}

bool ooh::init_script(script_data* ooh_data)
{
	return false;

	/*uti::u32 dll_name_len = (uti::u32)strnlen(ooh_data->dll_name, (size_t)script_data::max_dll_path);
	ooh_data->script_handle = uti::non_crypto_hash_32(&ooh_data->dll_name, dll_name_len);

	uti::ptr ooh_test_module = (uti::ptr)dlopen(ooh_data->dll_name, 0);
	if (ooh_test_module == 0)
		return false;
	//if (get_dll_path(ooh_test_module, ooh_data->dll_build_path, script_data::max_dll_path) == 0)
	//	return false;
	//if (!FreeLibrary(ooh_test_module))
	//	return false;

	uti::u64 dll_path_len = strnlen(ooh_data->dll_build_path, script_data::max_dll_path);
	memcpy(ooh_data->dll_load_path, ooh_data->dll_build_path, dll_path_len);
	char* ext = &ooh_data->dll_load_path[dll_path_len - 4];
	const char* in_use_end = "_temp.dll";
	memcpy(ext, in_use_end, strlen(in_use_end));

	memcpy(ooh_data->dll_func_prefix, ooh_data->dll_name, dll_name_len - 4);
	ext = &ooh_data->dll_name[dll_name_len - 4];
	memcpy(ext, in_use_end, strlen(in_use_end));

	if (!update_file_mod_time(ooh_data->dll_build_path, &ooh_data->dll_build_update_time))
		return false;

	return true;*/
}

bool ooh::load_script(script_data* ooh_data)
{
	return false;

	/*if (!CopyFileA(ooh_data->dll_build_path, ooh_data->dll_load_path, false))
		return false;
	ooh_data->dll_handle = (uti::ptr)LoadLibraryA(ooh_data->dll_name);
	uti::update_file_mod_time(ooh_data->dll_build_path, &ooh_data->dll_build_update_time);
	load_functions(ooh_data->dll_handle, ooh_data->dll_func_prefix, &ooh_data->functions);

	return true;*/
}

bool ooh::unload_script(script_data* ooh_data)
{
	return false;

	/*if (!FreeLibrary((HMODULE)ooh_data->dll_handle))
		return false;
	if (!DeleteFileA(ooh_data->dll_load_path))
		return false;

	ooh_data->dll_handle = 0;
	return true;*/
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
