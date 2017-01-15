#include "types.h"
#include "rearray.h"

#define OOH_EXPORT __declspec(dllexport)

extern "C"
{
	struct ooh_test_data
	{
		float offset;
	};
	
	uti::rearray<ooh_test_data> g_ooh_test_data_objects;

	OOH_EXPORT void	ooh_test_load_all(char* data, uti::u64 total_size)
	{
		uti::u64 num_items = *((uti::u64*)data);
		g_ooh_test_data_objects.reallocate(num_items, /*zero=*/false);
		data += sizeof(uti::u64);
		memcpy(g_ooh_test_data_objects.data, data, sizeof(ooh_test_data) * num_items);
	}

	OOH_EXPORT uti::u64 ooh_test_get_save_all_size()
	{
		return g_ooh_test_data_objects.size * sizeof(ooh_test_data) + sizeof(uti::u64);
	}

	OOH_EXPORT void	ooh_test_save_all(char* data, uti::u64 total_size)
	{
		assert(ooh_test_get_save_all_size() == total_size);
		auto num_items = (uti::u64*) data;
		*num_items = g_ooh_test_data_objects.size;
		data += sizeof(uti::u64);
		memcpy(data, g_ooh_test_data_objects.data, g_ooh_test_data_objects.size * sizeof(ooh_test_data));
	}

	OOH_EXPORT uti::u64 ooh_test_create()
	{
		ooh_test_data obj = ooh_test_data();
		g_ooh_test_data_objects.add_end(obj);
		return g_ooh_test_data_objects.size-1;
	}

	OOH_EXPORT void ooh_test_load(uti::u64 obj_id, char* data, uti::u64 size)
	{
		assert(size >= sizeof(ooh_test_data));
		memcpy(g_ooh_test_data_objects.data + obj_id, data, size);
	}

	OOH_EXPORT void ooh_test_save(uti::u64 obj_id, char* data, uti::u64 size)
	{
		assert(size >= sizeof(ooh_test_data));
		memcpy(g_ooh_test_data_objects.data + obj_id, data, size);
	}

	OOH_EXPORT uti::u64 ooh_test_data_size()
	{
		return sizeof(ooh_test_data);
	}

	OOH_EXPORT void ooh_test_start(uti::u64 obj_id)
	{

	}

	OOH_EXPORT void ooh_test_update(uti::u64 obj_id, float dt)
	{
		printf("%f\n", dt);
	}

	OOH_EXPORT void ooh_test_stop(uti::u64 obj_id)
	{

	}

	OOH_EXPORT void ooh_test_unload(uti::u64 obj_id)
	{

	}
}
