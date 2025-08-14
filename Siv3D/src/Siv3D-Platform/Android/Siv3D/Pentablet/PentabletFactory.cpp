//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/EngineOptions.hpp>
# include <Siv3D/Common.hpp>
# include <Siv3D/Pentablet/IPentablet.hpp>
# include "../Siv3D-Platform/Android/Siv3D/Pentablet/CPentablet.hpp"

namespace s3d
{
	ISiv3DPentablet* ISiv3DPentablet::Create()
	{
		return new CPentablet;
	}
} 
