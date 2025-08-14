//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//	Copyright (c) 2025 kestrel-90r
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	namespace OpenCV_Bridge
	{
		inline cv::Rect ToCVRect(const Rect& rect)
		{
			return{ rect.x, rect.y, rect.w, rect.h };
		}

		inline cv::Mat GetMatView(Image& image)
		{
#if SIV3D_PLATFORM(ANDROID)
            cv::Mat mat(image.height(), image.width(), CV_8UC4);
            std::memcpy(mat.data, image.dataAsUint8(), image.stride() * image.height());
            return mat;
#else
			return{ cv::Size{ image.width(), image.height() }, CV_8UC4, image.dataAsUint8(), image.stride() };
#endif
		}

		inline constexpr int32 ConvertBorderType(const BorderType borderType) noexcept
		{
			switch (borderType)
			{
			case BorderType::Replicate:
				return cv::BORDER_REPLICATE;
			//case BorderType::Wrap:
			//	return cv::BORDER_WRAP;
			case BorderType::Reflect:
				return cv::BORDER_REFLECT;
			case BorderType::Reflect_101:
				return cv::BORDER_REFLECT101;
			default:
				return cv::BORDER_DEFAULT;
			}
		}
	}
}
