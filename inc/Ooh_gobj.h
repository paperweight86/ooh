#pragma once

#include "types.h"

namespace Ooh
{
	// TODO: do we even want this in Ooh it seems out of place...
	struct game_object
	{
		uti::u64  shader;
		uti::u64  model;

		uti::float4   pos;
		uti::float4   rot;
		uti::float4   scale;
		uti::float44  transform;
		bool		  dirty;
	};
}
