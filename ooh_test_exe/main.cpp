
#include "types.h"
#include "ooh.h"
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include "Shlwapi.h"

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

int main(int argc, char** argv)
{
	ooh::ooh_dll_data ooh_data = {};
	const char* ooh_dll_name = "ooh_test.dll";
	memcpy(ooh_data.dll_name, ooh_dll_name, strlen(ooh_dll_name));
	ooh::ooh_initialise_dll(&ooh_data);

	ooh::ooh_load_dll(&ooh_data);

	uti::u64 test_obj_id = ooh_data.functions.create();
	ooh_data.functions.start(test_obj_id);

	while (true)
	{
		ooh_data.functions.update(test_obj_id, (float)(rand()%100000) / 100000.0f);
		Sleep(250);
		if (PathFileExistsA(ooh_data.dll_build_path))
		{ 
			uti::u64 new_update_time = 0;
			update_file_mod_time(ooh_data.dll_build_path, &new_update_time);
			if (new_update_time != ooh_data.dll_build_update_time)
			{
				ooh::ooh_reload_dll(&ooh_data);
			}
		}
	}

	ooh_data.functions.stop(test_obj_id);
	ooh_data.functions.unload(test_obj_id);

	ooh::ooh_unload_dll(&ooh_data);

	system("PAUSE");

	// So that's pretty fucking cool and easy... what do we try next?
	// Simplify loading code?
	// Write library code?
	// Test more things?
	// Make sure the interface is realistic? 

	return 0;
}
