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

#include <iostream>
#include <Siv3D/Common/Siv3DEngine.hpp>
#include <Siv3D/System/ISystem.hpp>
#include <Siv3D/Error.hpp>
#include <Siv3D/EngineLog.hpp>
#include <Siv3D/Window/IWindow.hpp>
#include <Siv3D.hpp>
#include <Siv3D/Renderer/IRenderer.hpp>
#include <Siv3D/Renderer2D/IRenderer2D.hpp>
#include <Siv3D/Renderer3D/IRenderer3D.hpp>
#include <Siv3D/Mesh/IMesh.hpp>
#include <Siv3D/PrimitiveMesh/IPrimitiveMesh.hpp>
#include <Siv3D/Shader/IShader.hpp>
#include <Siv3D/Font/IFont.hpp>
#include <Siv3D/ScreenCapture/IScreenCapture.hpp>
#include <Siv3D/Model/IModel.hpp>
#include <Siv3D/GUI/IGUI.hpp>
#include <Siv3D/Print/IPrint.hpp>
#include <Siv3D/Effect/IEffect.hpp>

#include <Siv3D/Renderer/GLES3/CRenderer_GLES3.hpp>
#include <Siv3D/UserAction/IUserAction.hpp>
#include <EGL/egl.h>

#define WEAK_SYMBOL __attribute__((weak))

/// AndroidアプリでResume未提供の場合の終了処理(デスクトップ版Siv3D互換)
/// @return 初期化が成功すれば true
WEAK_SYMBOL bool Init()
{
    LOG_ERROR(U"Default Init() called - application does not support resume. Exiting.");
    SIV3D_ENGINE(UserAction)->reportUserActions(UserAction::SystemExitCalled);
    return false;
}

void Main();

namespace s3d::detail::init
{
    void InitCommandLines(int argc, char **argv);
    void InitModulePath(const char *arg);
}

bool g_siv3dRunning = false;

/// エンジンの実行状態取得
/// @return 実行中なら true
bool isSiv3DRunning()
{
    return g_siv3dRunning;
}

bool g_isSuspending = false;
bool g_isAwaitingResume = false;
std::mutex g_CallbackMutex;

bool g_isRenderingSuspended = false;

namespace s3d
{
    /// サスペンド時に保存する描画状態のスナップショット
    struct SuspendedState
    {
        Size windowSize;
        Size sceneSize;
        ColorF backgroundColor;
        bool isValid = false;
    };

    static SuspendedState g_suspendedState;
}

/// サスペンド開始処理
void OnSuspend()
{
    std::lock_guard<std::mutex> lock(g_CallbackMutex);

    if (!g_isSuspending) return;

    LOG_INFO(U"OnSuspend: Saving current state");

    s3d::g_suspendedState.windowSize = Window::GetState().virtualSize;
    s3d::g_suspendedState.sceneSize = Scene::Size();
    s3d::g_suspendedState.backgroundColor = Scene::GetBackground();
    s3d::g_suspendedState.isValid = true;

    LOG_INFO(U"Saved state - Window: {}x{}, Scene: {}x{}"_fmt(
        s3d::g_suspendedState.windowSize.x, s3d::g_suspendedState.windowSize.y,
        s3d::g_suspendedState.sceneSize.x, s3d::g_suspendedState.sceneSize.y));

    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    LOG_INFO(U"OnSuspend: Releasing assets");

    for (const auto &[name, info] : TextureAsset::Enumerate())
    {
        LOG_INFO(U"TextureAsset::Release({})"_fmt(name));
        TextureAsset::Release(name);
    }

    for (const auto &[name, info] : FontAsset::Enumerate())
    {
        LOG_INFO(U"FontAsset::Release({})"_fmt(name));
        FontAsset::Release(name);
    }

    for (const auto &[name, info] : AudioAsset::Enumerate())
    {
        LOG_INFO(U"AudioAsset::Release({})"_fmt(name));
        AudioAsset::Release(name);
    }

    g_isSuspending = false;
    LOG_INFO(U"OnSuspend: Complete");
}

/// テクスチャ有効判定
/// @return 正常なら true
bool isTextureValid()
{
    try
    {
        const Size size = SIV3D_ENGINE(Texture)->getSize(Texture::IDType::NullAsset());
        return true;
    }
    catch (...)
    {
        LOG_ERROR(U"Texture system validation failed");
        return false;
    }
}

/// フォント有効判定
/// @return 正常なら true
bool isFontValid()
{
    try
    {
        const size_t count = SIV3D_ENGINE(Font)->getFontCount();

        const auto &prop = SIV3D_ENGINE(Font)->getProperty(Font::IDType::NullAsset());
        return true;
    }
    catch (...)
    {
        LOG_ERROR(U"Font system validation failed");
        return false;
    }
}

/// 描画可能待機(ブロッキング)
/// @param maxRetries 最大リトライ回数
void BlockingForReady(const int maxRetries = 10)
{
    int retryCount = 0;

    while (((!isTextureValid()) || (!isFontValid())) && (retryCount < maxRetries))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        retryCount++;

        if ((retryCount % 5) == 0 || retryCount == 1)
        {
            LOG_INFO(U"Waiting for resources... attempt {}/{}"_fmt(retryCount, maxRetries));
        }
    }

    if (retryCount >= maxRetries)
    {
        LOG_ERROR(U"Resource initialization timed out after {} attempts"_fmt(maxRetries));
    }
    else
    {
        LOG_INFO(U"All resources initialized successfully after {} attempts"_fmt(retryCount));
    }
}


/// レジューム処理
void OnResume()
{
    LOG_INFO(U"OnResume: Starting...");

    FontAsset::UnregisterAll();
    TextureAsset::UnregisterAll();

    SIV3D_ENGINE(Print)->clear();
    SIV3D_ENGINE(Renderer3D)->deinit();
    SIV3D_ENGINE(Renderer2D)->deinit();
    SIV3D_ENGINE(Renderer)->deinit();
    SIV3D_ENGINE(Font)->deinit();
    SIV3D_ENGINE(GUI)->deinit();
    SIV3D_ENGINE(Mesh)->deinit();
    SIV3D_ENGINE(PrimitiveMesh)->deinit();
    SIV3D_ENGINE(Texture)->deinit();

    SIV3D_ENGINE(Renderer)->init();
    SIV3D_ENGINE(Renderer2D)->init();
    SIV3D_ENGINE(Renderer3D)->init();

    SIV3D_ENGINE(ScreenCapture)->init();
    SIV3D_ENGINE(Model)->init();
    SIV3D_ENGINE(Font)->init();
    SIV3D_ENGINE(GUI)->init();
    SIV3D_ENGINE(Print)->init();
    SIV3D_ENGINE(PrimitiveMesh)->init();
    SIV3D_ENGINE(Effect)->init();

    LOG_INFO(U"OnResume: Renderer reinitialized successfully");

    if (s3d::g_suspendedState.isValid)
    {
        LOG_INFO(U"Restoring saved state - Window: {}x{}, Scene: {}x{}"_fmt(
            s3d::g_suspendedState.windowSize.x, s3d::g_suspendedState.windowSize.y,
            s3d::g_suspendedState.sceneSize.x, s3d::g_suspendedState.sceneSize.y));

        Window::Resize(s3d::g_suspendedState.windowSize);
        Scene::Resize(s3d::g_suspendedState.sceneSize);
        Scene::SetBackground(s3d::g_suspendedState.backgroundColor);
    }
    else
    {
        LOG_INFO(U"No saved state found, using defaults");
        Window::Resize(800, 600);
        Scene::Resize(800, 600);
        Scene::SetBackground(ColorF{0.6, 0.8, 0.7});
    }

    SIV3D_ENGINE(Print)->clear();

    BlockingForReady();

    if (!Init())
    {
        LOG_ERROR(U"Init() failed. Application may be unstable.");
    }
}

/// レジューム開始処理
void StartResume()
{
	if( g_isRenderingSuspended )
    {
	    std::lock_guard<std::mutex> lock(g_CallbackMutex);
	    g_isAwaitingResume = true;
	}
}

/// サスペンド開始処理
void StartSuspend()
{
    std::lock_guard<std::mutex> lock(g_CallbackMutex);
    g_isRenderingSuspended = true;
    g_isSuspending = true;
}

/// Android エントリポイント（Siv3D エンジン初期化と Main 実行）
/// @param argc 引数個数
/// @param argv 引数配列
/// @param width 初期フレームバッファ幅
/// @param height 初期フレームバッファ高
/// @return 正常終了で 0、失敗時は負値
extern "C" int Siv3DMain(int argc, char *argv[], int width, int height)
{
    using namespace s3d;
    LOG_TRACE(U"OpenSiv3D for Android");
    g_siv3dRunning = false;
    detail::init::InitCommandLines(argc, argv);
    detail::init::InitModulePath(argv[0]);

    Siv3DEngine engine;

    LOG_TRACE(U"Initializing OpenSiv3D");
    try
    {
        SIV3D_ENGINE(Window)->resizeByFrameBufferSize({width, height});
        SIV3D_ENGINE(System)->init();

        Window::Resize(800, 600);
        Scene::Resize(800, 600);
        g_siv3dRunning = true;
    }
    catch (const Error &error)
    {
        std::cerr << error << '\n';
        LOG_TRACE(U"Critical error during SIV3D_ENGINE(System)->init()");
        return -1;
    }

    LOG_TRACE(U"Main() ---");

    Main();

    LOG_TRACE(U"--- Main()");

    return 0;
}
