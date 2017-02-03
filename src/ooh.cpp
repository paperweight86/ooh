#include "tat.h"
#include "types.h"

#include "ooh.h"
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

#include "non_crypto_hash.h"

namespace
{
	static bool update_file_mod_time(const char* filepath, uti::u64* update_time)
	{
		HANDLE file_handle;
		FILETIME mod_time;
		file_handle = CreateFile(filepath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (file_handle == INVALID_HANDLE_VALUE)
		{
			return false;
		}

		if (!GetFileTime(file_handle, NULL, NULL, &mod_time))
		{
			CloseHandle(file_handle);
			return false;
		}

		CloseHandle(file_handle);

		uti::u64 mod_time_64 = (uti::u64)mod_time.dwLowDateTime | ((uti::u64)mod_time.dwHighDateTime << 32);

		bool changed = mod_time_64 > *update_time;

		*update_time = mod_time_64;

		return changed;
	}
}

bool ooh::load_functions(uti::ptr dll_handle, const char* function_prefix, behaviour_functions* table)
{
	bool all_loaded = true;

	const int buffer_size = 256;
	char buffer[buffer_size];

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "load_all");
	table->load_all = (ooh_load_all_signature*)GetProcAddress((HMODULE)dll_handle, buffer);
	if (table->load_all == nullptr)
		all_loaded = false;

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "get_save_all_size");
	table->save_all_size = (ooh_get_save_all_size_signature*)GetProcAddress((HMODULE)dll_handle, buffer);
	if (table->save_all_size == nullptr)
		all_loaded = false;

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "save_all");
	table->save_all = (ooh_save_all_signature*)GetProcAddress((HMODULE)dll_handle, buffer);
	if (table->save_all == nullptr)
		all_loaded = false;

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "create");
	table->create = (ooh_create_signature*)GetProcAddress((HMODULE)dll_handle, buffer);
	if (table->create == nullptr)
		all_loaded = false;

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "load");
	table->load = (ooh_load_signature*)GetProcAddress((HMODULE)dll_handle, buffer);
	if (table->load == nullptr)
		all_loaded = false;

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "save");
	table->save = (ooh_save_signature*)GetProcAddress((HMODULE)dll_handle, buffer);
	if (table->save == nullptr)
		all_loaded = false;

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "data_size");
	table->data_size = (ooh_data_size_signature*)GetProcAddress((HMODULE)dll_handle, buffer);
	if (table->data_size == nullptr)
		all_loaded = false;

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "start");
	table->start = (ooh_start_signature*)GetProcAddress((HMODULE)dll_handle, buffer);
	if (table->start == nullptr)
		all_loaded = false;

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "update");
	table->update = (ooh_update_signature*)GetProcAddress((HMODULE)dll_handle, buffer);
	if (table->update == nullptr)
		all_loaded = false;

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "stop");
	table->stop = (ooh_stop_signature*)GetProcAddress((HMODULE)dll_handle, buffer);
	if (table->stop == nullptr)
		all_loaded = false;

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "unload");
	table->unload = (ooh_unload_signature*)GetProcAddress((HMODULE)dll_handle, buffer);
	if (table->unload == nullptr)
		all_loaded = false;

	return !all_loaded;
}

bool ooh::init_script(script_data* ooh_data)
{
	uti::u32 dll_name_len = (uti::u32)strnlen(ooh_data->dll_name, (size_t)script_data::max_dll_path);
	ooh_data->script_handle = uti::non_crypto_hash_32(&ooh_data->dll_name, dll_name_len);

	HMODULE ooh_test_module = LoadLibraryA(ooh_data->dll_name);
	if (ooh_test_module == NULL)
		return false;
	if (GetModuleFileNameA(ooh_test_module, ooh_data->dll_build_path, script_data::max_dll_path) == 0)
		return false;
	if (!FreeLibrary(ooh_test_module))
		return false;

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

	return true;
}

bool ooh::load_script(script_data* ooh_data)
{
	if (!CopyFileA(ooh_data->dll_build_path, ooh_data->dll_load_path, false))
		return false;
	ooh_data->dll_handle = (uti::ptr)LoadLibraryA(ooh_data->dll_name);
	update_file_mod_time(ooh_data->dll_build_path, &ooh_data->dll_build_update_time);
	load_functions(ooh_data->dll_handle, ooh_data->dll_func_prefix, &ooh_data->functions);

	return true;
}

bool ooh::unload_script(script_data* ooh_data)
{
	if (!FreeLibrary((HMODULE)ooh_data->dll_handle))
		return false;
	if (!DeleteFileA(ooh_data->dll_load_path))
		return false;

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
