#include "tat.h"
#include "types.h"

#include "Ooh.h"
#ifdef TAT_WINDOWS
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

bool Ooh::load_functions(uti::ptr dll_handle, const char* function_prefix, behaviour_functions* table)
{
	bool all_loaded = true;

	const int buffer_size = 256;
	char buffer[buffer_size];

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "load_all");
	table->load_all = (Ooh_load_all_signature*)GetProcAddress((HMODULE)dll_handle, buffer);
	if (table->load_all == nullptr)
		all_loaded = false;

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "get_save_all_size");
	table->save_all_size = (Ooh_get_save_all_size_signature*)GetProcAddress((HMODULE)dll_handle, buffer);
	if (table->save_all_size == nullptr)
		all_loaded = false;

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "save_all");
	table->save_all = (Ooh_save_all_signature*)GetProcAddress((HMODULE)dll_handle, buffer);
	if (table->save_all == nullptr)
		all_loaded = false;

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "create");
	table->create = (Ooh_create_signature*)GetProcAddress((HMODULE)dll_handle, buffer);
	if (table->create == nullptr)
		all_loaded = false;

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "load");
	table->load = (Ooh_load_signature*)GetProcAddress((HMODULE)dll_handle, buffer);
	if (table->load == nullptr)
		all_loaded = false;

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "save");
	table->save = (Ooh_save_signature*)GetProcAddress((HMODULE)dll_handle, buffer);
	if (table->save == nullptr)
		all_loaded = false;

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "data_size");
	table->data_size = (Ooh_data_size_signature*)GetProcAddress((HMODULE)dll_handle, buffer);
	if (table->data_size == nullptr)
		all_loaded = false;

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "start");
	table->start = (Ooh_start_signature*)GetProcAddress((HMODULE)dll_handle, buffer);
	if (table->start == nullptr)
		all_loaded = false;

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "update");
	table->update = (Ooh_update_signature*)GetProcAddress((HMODULE)dll_handle, buffer);
	if (table->update == nullptr)
		all_loaded = false;

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "stop");
	table->stop = (Ooh_stop_signature*)GetProcAddress((HMODULE)dll_handle, buffer);
	if (table->stop == nullptr)
		all_loaded = false;

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "unload");
	table->unload = (Ooh_unload_signature*)GetProcAddress((HMODULE)dll_handle, buffer);
	if (table->unload == nullptr)
		all_loaded = false;

	return !all_loaded;
}

bool Ooh::init_script(script_data* Ooh_data)
{
	uti::u32 dll_name_len = (uti::u32)strnlen(Ooh_data->dll_name, (size_t)script_data::max_dll_path);
	Ooh_data->script_handle = uti::non_crypto_hash_32(&Ooh_data->dll_name, dll_name_len);

	HMODULE Ooh_test_module = LoadLibraryA(Ooh_data->dll_name);
	if (Ooh_test_module == NULL)
		return false;
	if (GetModuleFileNameA(Ooh_test_module, Ooh_data->dll_build_path, script_data::max_dll_path) == 0)
	{
		FreeLibrary(Ooh_test_module);
		return false;
	}
	if (!FreeLibrary(Ooh_test_module))
		return false;

	uti::u64 dll_path_len = strnlen(Ooh_data->dll_build_path, script_data::max_dll_path);
	memcpy(Ooh_data->dll_load_path, Ooh_data->dll_build_path, dll_path_len);
	char* ext = &Ooh_data->dll_load_path[dll_path_len - 4];
	const char* in_use_end = "_temp.dll";
	memcpy(ext, in_use_end, strlen(in_use_end));

	memcpy(Ooh_data->dll_func_prefix, Ooh_data->dll_name, dll_name_len - 4);
	ext = &Ooh_data->dll_name[dll_name_len - 4];
	memcpy(ext, in_use_end, strlen(in_use_end));

	update_file_mod_time(Ooh_data->dll_build_path, &Ooh_data->dll_build_update_time);

	return true;
}

bool Ooh::load_script(script_data* Ooh_data)
{
	if (!CopyFileA(Ooh_data->dll_build_path, Ooh_data->dll_load_path, false))
		return false;
	Ooh_data->dll_handle = (uti::ptr)LoadLibraryA(Ooh_data->dll_name);
	update_file_mod_time(Ooh_data->dll_build_path, &Ooh_data->dll_build_update_time);
	load_functions(Ooh_data->dll_handle, Ooh_data->dll_func_prefix, &Ooh_data->functions);

	return true;
}

bool Ooh::unload_script(script_data* Ooh_data)
{
	if (!FreeLibrary((HMODULE)Ooh_data->dll_handle))
		return false;
	if (!DeleteFileA(Ooh_data->dll_load_path))
		return false;

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
