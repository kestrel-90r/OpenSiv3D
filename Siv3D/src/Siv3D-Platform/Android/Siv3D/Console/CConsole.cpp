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

# include <ios>
# include <Siv3D/Common.hpp>
# include <Siv3D/EngineLog.hpp>
# include <android/log.h>
# include "CConsole.hpp"

namespace s3d
{
	CConsole::CConsole()
	{
		LOG_SCOPED_TRACE(U"CConsole::CConsole()");
		std::ios_base::sync_with_stdio(false);
	}

	CConsole::~CConsole()
	{
		LOG_SCOPED_TRACE(U"CConsole::~CConsole()");
	}

	void CConsole::open()
	{
		// Androidではコンソールウィンドウを開く代わりにログ出力を有効化
		LOG_INFO(U"ℹ️ Console opened");
	}

	void CConsole::close()
	{
		// Androidではコンソールウィンドウを閉じる代わりにログ出力を無効化
		LOG_INFO(U"ℹ️ Console closed");
	}

	void CConsole::setSystemDefaultCodePage()
	{
		// Androidでは何もしない（UTF-8が標準）
		LOG_INFO(U"ℹ️ Console: setSystemDefaultCodePage() called (no effect on Android)");
	}

	void CConsole::setUTF8CodePage()
	{
		// Androidでは何もしない（UTF-8が標準）
		LOG_INFO(U"ℹ️ Console: setUTF8CodePage() called (no effect on Android)");
	}
}
