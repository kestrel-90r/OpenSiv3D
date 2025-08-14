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
	class Font;
	class Texture;

	class SIV3D_NOVTABLE ISiv3DGUI
	{
	public:

		static ISiv3DGUI* Create();

		virtual ~ISiv3DGUI() = default;

		virtual void init() = 0;

# if SIV3D_PLATFORM(ANDROID)
		virtual void deinit() = 0;
#endif

		virtual const Font& getDefaultFont() const noexcept = 0;

		virtual const Texture& getColorPickerTexture() = 0;
	};
}
