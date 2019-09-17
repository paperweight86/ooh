#pragma once

namespace Ooh
{
	typedef void Ooh_load_all_signature(char* data, uti::u64 total_size);
	typedef uti::u64 Ooh_get_save_all_size_signature();
	typedef void Ooh_save_all_signature(char* data, uti::u64 total_size);

	typedef uti::u64 Ooh_create_signature();
	typedef void	 Ooh_load_signature(uti::u64 obj_id, char* data, uti::u64 size);
	typedef void	 Ooh_save_signature(uti::u64 obj_id, char* data, uti::u64 size);
	typedef uti::u64 Ooh_data_size_signature();
	typedef void	 Ooh_start_signature(uti::u64 obj_id);
	typedef void	 Ooh_update_signature(uti::u64 obj_id, float dt);
	typedef void	 Ooh_stop_signature(uti::u64 obj_id);
	typedef void	 Ooh_unload_signature(uti::u64 obj_id);

	struct behaviour_functions
	{
		Ooh_load_all_signature*				load_all;
		Ooh_get_save_all_size_signature*	save_all_size;
		Ooh_save_all_signature*				save_all;

		Ooh_create_signature*	 create;
		Ooh_load_signature*		 load;
		Ooh_save_signature*		 save;
		Ooh_data_size_signature* data_size;
		Ooh_start_signature*	 start;
		Ooh_update_signature*	 update;
		Ooh_stop_signature*		 stop;
		Ooh_unload_signature*	 unload;
	};

	typedef uti::u64 script_inst_handle;
	typedef uti::u32 script_handle;

	struct script_data
	{
		static const uti::u32 max_dll_path = 260;

		char dll_func_prefix[max_dll_path];

		// the name of the dll to load
		char dll_name[max_dll_path];

		// path dll is built to
		char dll_build_path[max_dll_path];
		uti::u64 dll_build_update_time;
		// path dll is copied to before being loaded
		char dll_load_path[max_dll_path];

		// handle when the dll is loaded
		uti::ptr dll_handle;

		// the unique id of this script
		script_handle script_handle;

		behaviour_functions functions;
	};

	bool load_functions(uti::ptr dll_handle, const char* function_prefix, behaviour_functions* table);

	bool init_script(script_data* Ooh_data);

	bool load_script(script_data* Ooh_data);

	bool unload_script(script_data* Ooh_data);

	bool reload_script(script_data* Ooh_data);
}
