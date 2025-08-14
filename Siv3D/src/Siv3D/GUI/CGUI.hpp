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
# include <Siv3D/Font.hpp>
# include <Siv3D/Texture.hpp>
# include "IGUI.hpp"

namespace s3d
{
	class CGUI final : public ISiv3DGUI
	{
	public:

		CGUI();

		~CGUI() override;

		void init() override;

# if SIV3D_PLATFORM(ANDROID)
		void deinit() override;
#endif

		const Font& getDefaultFont() const noexcept override;

		const Texture& getColorPickerTexture() override;

	private:

		std::unique_ptr<Font> m_defaultFont;

		Array<Font> m_iconFonts;

		std::unique_ptr<Texture> m_colorPickerTexture;
	};
}
