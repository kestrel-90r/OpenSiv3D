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
# include <jni.h>

# include <Siv3D/Window.hpp>
# include <Siv3D/String.hpp>
# include <Siv3D/WindowState.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Common/OpenGLES.hpp>
# include <Siv3D/Common.hpp>

namespace s3d
{
	class CWindow final : public ISiv3DWindow
	{
	public:
		CWindow();
		~CWindow() override;

		void init() override;
		void update() override;
		void setWindowTitle(const String& title) override;
		const String& getWindowTitle() const noexcept override;
		void* getHandle() const noexcept override;
		const WindowState& getState() const noexcept override;
		void setStyle(WindowStyle style) override;
		void setPos(const Point& pos) override;
		void maximize() override;
		void restore() override;
		void minimize() override;
		bool resizeByVirtualSize(const Size& size) override;
		bool resizeByFrameBufferSize(const Size& size) override;
		void setMinimumFrameBufferSize(const Size& size) override;
		void setFullscreen(bool fullscreen, size_t monitorIndex) override;
		void setToggleFullscreenEnabled(bool enabled) override;
		bool isToggleFullscreenEnabled() const override;

# if SIV3D_PLATFORM(ANDROID)
        void* getNativeWindow() const noexcept override;
#endif
		void updatePos(const Point& pos);
		void updateSize(const Size& size);
		void updateFrameBufferSize(const Size& size);
		void updateScaling(double scaling);
		void updateFocus(bool focused);

	private:
		void updateState();

		WindowState m_state;
		String m_title;
		void* m_window = nullptr;

	};

}

# if SIV3D_PLATFORM(ANDROID)
// Forward declare the JNI functions with C linkage
extern "C" {
	JNIEXPORT void JNICALL Java_com_kestrel_opensiv3d_MainActivity_onMoveNative(JNIEnv*, jobject, jint, jint);
	JNIEXPORT void JNICALL Java_com_kestrel_opensiv3d_MainActivity_onResizeNative(JNIEnv*, jobject, jint, jint);
	JNIEXPORT void JNICALL Java_com_kestrel_opensiv3d_MainActivity_onFrameBufferSizeNative(JNIEnv*, jobject, jint, jint);
	JNIEXPORT void JNICALL Java_com_kestrel_opensiv3d_MainActivity_onScalingChangeNative(JNIEnv*, jobject, jfloat, jfloat);
	JNIEXPORT void JNICALL Java_com_kestrel_opensiv3d_MainActivity_onPauseNative(JNIEnv*, jobject);
	JNIEXPORT void JNICALL Java_com_kestrel_opensiv3d_MainActivity_onResumeNative(JNIEnv*, jobject);
	JNIEXPORT void JNICALL Java_com_kestrel_opensiv3d_MainActivity_SetMainActivityNative(JNIEnv*, jobject);
}
#endif