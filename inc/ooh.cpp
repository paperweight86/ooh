#include "types.h"
#include "ooh.h"
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

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

void ooh::load_ooh_functions(uti::ptr dll_ptr, const char* function_prefix, ooh_behaviour_functions* table)
{
	const int buffer_size = 256;
	char buffer[buffer_size];

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "load_all");
	table->load_all = (ooh_load_all_signature*)GetProcAddress((HMODULE)dll_ptr, buffer);

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "save_all_size");
	table->save_all_size = (ooh_get_save_all_size_signature*)GetProcAddress((HMODULE)dll_ptr, buffer);

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "save_all");
	table->save_all = (ooh_save_all_signature*)GetProcAddress((HMODULE)dll_ptr, buffer);

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "create");
	table->create = (ooh_create_signature*)GetProcAddress((HMODULE)dll_ptr, buffer);

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "load");
	table->load = (ooh_load_signature*)GetProcAddress((HMODULE)dll_ptr, buffer);

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "save");
	table->save = (ooh_save_signature*)GetProcAddress((HMODULE)dll_ptr, buffer);

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "data_size");
	table->data_size = (ooh_data_size_signature*)GetProcAddress((HMODULE)dll_ptr, buffer);

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "start");
	table->start = (ooh_start_signature*)GetProcAddress((HMODULE)dll_ptr, buffer);

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "update");
	table->update = (ooh_update_signature*)GetProcAddress((HMODULE)dll_ptr, buffer);

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "stop");
	table->stop = (ooh_stop_signature*)GetProcAddress((HMODULE)dll_ptr, buffer);

	sprintf_s<buffer_size>(buffer, "%s_%s", function_prefix, "unload");
	table->unload = (ooh_unload_signature*)GetProcAddress((HMODULE)dll_ptr, buffer);
}

void ooh::ooh_initialise_dll(ooh_dll_data* ooh_data)
{
	HMODULE ooh_test_module = LoadLibraryA(ooh_data->dll_name);
	GetModuleFileNameA(ooh_test_module, ooh_data->dll_build_path, ooh_dll_data::max_dll_path);
	FreeLibrary(ooh_test_module);

	uti::u64 dll_path_len = strnlen(ooh_data->dll_build_path, ooh_dll_data::max_dll_path);
	memcpy(ooh_data->dll_load_path, ooh_data->dll_build_path, dll_path_len);
	char* ext = &ooh_data->dll_load_path[dll_path_len - 4];
	const char* in_use_end = "_temp.dll";
	memcpy(ext, in_use_end, strlen(in_use_end));

	uti::u64 dll_name_len = strnlen(ooh_data->dll_name, ooh_dll_data::max_dll_path);
	memcpy(ooh_data->dll_func_prefix, ooh_data->dll_name, dll_name_len - 4);
	ext = &ooh_data->dll_name[dll_name_len - 4];
	memcpy(ext, in_use_end, strlen(in_use_end));

	update_file_mod_time(ooh_data->dll_build_path, &ooh_data->dll_build_update_time);
}

void ooh::ooh_load_dll(ooh_dll_data* ooh_data)
{
	CopyFile(ooh_data->dll_build_path, ooh_data->dll_load_path, false);
	ooh_data->dll_handle = (uti::ptr)LoadLibraryA(ooh_data->dll_name);
	update_file_mod_time(ooh_data->dll_build_path, &ooh_data->dll_build_update_time);
	load_ooh_functions(ooh_data->dll_handle, ooh_data->dll_func_prefix, &ooh_data->functions);
}

void ooh::ooh_unload_dll(ooh_dll_data* ooh_data)
{
	FreeLibrary((HMODULE)ooh_data->dll_handle);
	DeleteFileA(ooh_data->dll_load_path);
	ooh_data->dll_handle = 0;
}

void ooh::ooh_reload_dll(ooh_dll_data* ooh_data)
{
	ooh_unload_dll(ooh_data);
	ooh_load_dll(ooh_data);
}
