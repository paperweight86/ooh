#pragma once

namespace ooh
{
	typedef void ooh_load_all_signature(char* data, uti::u64 total_size);
	typedef uti::u64 ooh_get_save_all_size_signature();
	typedef void ooh_save_all_signature(char* data, uti::u64 total_size);

	typedef uti::u64 ooh_create_signature();
	typedef void	 ooh_load_signature(uti::u64 obj_id, char* data, uti::u64 size);
	typedef void	 ooh_save_signature(uti::u64 obj_id, char* data, uti::u64 size);
	typedef uti::u64 ooh_data_size_signature();
	typedef void	 ooh_start_signature(uti::u64 obj_id);
	typedef void	 ooh_update_signature(uti::u64 obj_id, float dt);
	typedef void	 ooh_stop_signature(uti::u64 obj_id);
	typedef void	 ooh_unload_signature(uti::u64 obj_id);

	struct behaviour_functions
	{
		ooh_load_all_signature*				load_all;
		ooh_get_save_all_size_signature*	save_all_size;
		ooh_save_all_signature*				save_all;

		ooh_create_signature*	 create;
		ooh_load_signature*		 load;
		ooh_save_signature*		 save;
		ooh_data_size_signature* data_size;
		ooh_start_signature*	 start;
		ooh_update_signature*	 update;
		ooh_stop_signature*		 stop;
		ooh_unload_signature*	 unload;
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

	bool init_script(script_data* ooh_data);

	bool load_script(script_data* ooh_data);

	bool unload_script(script_data* ooh_data);

	bool reload_script(script_data* ooh_data);
}
