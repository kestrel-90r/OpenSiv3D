//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//	Copyright (c) 2025      kestrel-90r
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <Siv3D/Common.hpp>

namespace s3d
{
	class IConstantBufferDetail
	{
	public:

		static IConstantBufferDetail* Create(size_t size);

		virtual ~IConstantBufferDetail() = default;

	public:

		virtual bool update(const void* data, size_t size) = 0;
		
# if SIV3D_PLATFORM(ANDROID)
		virtual void destroy() = 0;
#endif
	};
}
