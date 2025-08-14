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

#include <Siv3D/Logger.hpp>
#include <Siv3D/Error.hpp>
#include <Siv3D/EngineLog.hpp>
#include <Siv3D/Unicode.hpp>
#include <Siv3D/WindowState.hpp>
#include <Siv3D/FormatLiteral.hpp>
#include <Siv3D/Window/IWindow.hpp>
#include <Siv3D/Texture/ITexture.hpp>
#include <Siv3D/Shader/IShader.hpp>
#include <Siv3D/Mesh/IMesh.hpp>
#include <Siv3D/Renderer2D/GLES3/CRenderer2D_GLES3.hpp>
#include <Siv3D/Renderer3D/GLES3/CRenderer3D_GLES3.hpp>
#include <Siv3D/Common/Siv3DEngine.hpp>

#if SIV3D_PLATFORM(ANDROID)
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <Siv3D/Renderer/GLES3/CRenderer_GLES3.hpp>
#include <EGL/egl.h>
#endif

namespace s3d
{
    static const char *getEGLErrorString(EGLint error)
    {
        switch (error)
        {
        case EGL_SUCCESS:
            return "EGL_SUCCESS";
        case EGL_NOT_INITIALIZED:
            return "EGL_NOT_INITIALIZED";
        case EGL_BAD_ACCESS:
            return "EGL_BAD_ACCESS";
        case EGL_BAD_ALLOC:
            return "EGL_BAD_ALLOC";
        case EGL_BAD_ATTRIBUTE:
            return "EGL_BAD_ATTRIBUTE";
        case EGL_BAD_CONTEXT:
            return "EGL_BAD_CONTEXT";
        case EGL_BAD_CONFIG:
            return "EGL_BAD_CONFIG";
        case EGL_BAD_CURRENT_SURFACE:
            return "EGL_BAD_CURRENT_SURFACE";
        case EGL_BAD_DISPLAY:
            return "EGL_BAD_DISPLAY";
        case EGL_BAD_SURFACE:
            return "EGL_BAD_SURFACE";
        case EGL_BAD_MATCH:
            return "EGL_BAD_MATCH";
        case EGL_BAD_PARAMETER:
            return "EGL_BAD_PARAMETER";
        case EGL_BAD_NATIVE_PIXMAP:
            return "EGL_BAD_NATIVE_PIXMAP";
        case EGL_BAD_NATIVE_WINDOW:
            return "EGL_BAD_NATIVE_WINDOW";
        case EGL_CONTEXT_LOST:
            return "EGL_CONTEXT_LOST";
        default:
            return "Unknown EGL error";
        }
    }

    CRenderer_GLES3::CRenderer_GLES3()
    {
    }

    CRenderer_GLES3::~CRenderer_GLES3()
    {
        LOG_SCOPED_TRACE(U"CRenderer_GLES3::~CRenderer_GLES3()");
    }

    EngineOption::Renderer CRenderer_GLES3::getRendererType() const noexcept
    {
        return EngineOption::Renderer::WebGL2;
    }

    void CRenderer_GLES3::init()
    {
        LOG_SCOPED_TRACE(U"CRenderer_GLES3::init()");

        pRenderer2D = static_cast<CRenderer2D_GLES3 *>(SIV3D_ENGINE(Renderer2D));
        pRenderer3D = static_cast<CRenderer3D_GLES3 *>(SIV3D_ENGINE(Renderer3D));

        try
        {
            m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
            if (m_display == EGL_NO_DISPLAY)
            {
                EGLint error = eglGetError();
                LOG_ERROR(U"eglGetDisplay() failed: {}"_fmt(Unicode::Widen(getEGLErrorString(error))));
                throw EngineError(U"eglGetDisplay() failed");
            }

            EGLint eglMajor, eglMinor;
            if (eglInitialize(m_display, &eglMajor, &eglMinor) != EGL_TRUE)
            {
                EGLint error = eglGetError();
                LOG_ERROR(U"eglInitialize() failed: {}"_fmt(Unicode::Widen(getEGLErrorString(error))));
                throw EngineError(U"eglInitialize() failed");
            }

            EGLint configAttribs[] = {
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                EGL_BLUE_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_RED_SIZE, 8,
                EGL_DEPTH_SIZE, 24,
                EGL_NONE};

            EGLint numConfigs;
            if (eglChooseConfig(m_display, configAttribs, &m_config, 1, &numConfigs) != EGL_TRUE || numConfigs < 1)
            {
                EGLint error = eglGetError();
                LOG_ERROR(U"eglChooseConfig() failed: {}"_fmt(Unicode::Widen(getEGLErrorString(error))));
                throw EngineError(U"eglChooseConfig() failed");
            }

            EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
            m_context = eglCreateContext(m_display, m_config, EGL_NO_CONTEXT, contextAttribs);
            if (m_context == EGL_NO_CONTEXT)
            {
                EGLint error = eglGetError();
                LOG_ERROR(U"eglCreateContext() failed: {}"_fmt(Unicode::Widen(getEGLErrorString(error))));
                throw EngineError(U"eglCreateContext() failed");
            }

            // Create a surface - get the native window handle from Window interface
            void *windowHandle = SIV3D_ENGINE(Window)->getNativeWindow();
            LOG_INFO(U"Getting native window handle: {}"_fmt(static_cast<size_t>(reinterpret_cast<std::uintptr_t>(windowHandle))));

            ANativeWindow *nativeWindow = static_cast<ANativeWindow *>(windowHandle);

            if (!nativeWindow)
            {
                LOG_ERROR(U"ANativeWindow is null. Native window not available.");
                LOG_ERROR(U"Current window handle from IWindow: {}"_fmt(static_cast<size_t>(reinterpret_cast<std::uintptr_t>(SIV3D_ENGINE(Window)->getHandle()))));

                // とりあえずループして待機してみる
                for (int i = 0; i < 5; i++)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));

                    windowHandle = SIV3D_ENGINE(Window)->getNativeWindow();
                    nativeWindow = static_cast<ANativeWindow *>(windowHandle);
                    LOG_INFO(U"Retry[{}] getting native window: {}"_fmt(i, static_cast<size_t>(reinterpret_cast<std::uintptr_t>(nativeWindow))));

                    if (nativeWindow)
                    {
                        break;
                    }
                }

                if (!nativeWindow)
                {
                    throw EngineError(U"ANativeWindow is null. Native window not available after retries.");
                }
            }

            LOG_INFO(U"Creating EGL surface with window handle: {}"_fmt(static_cast<size_t>(reinterpret_cast<std::uintptr_t>(nativeWindow))));

            m_eglSurface = eglCreateWindowSurface(m_display, m_config, nativeWindow, nullptr);

            if (m_eglSurface == EGL_NO_SURFACE)
            {
                EGLint error = eglGetError();
                String errorMessage;

                switch (error)
                {
                case EGL_BAD_NATIVE_WINDOW:
                    errorMessage = U"EGL_BAD_NATIVE_WINDOW: The ANativeWindow handle does not refer to a valid window.";
                    break;
                case EGL_BAD_ALLOC:
                    errorMessage = U"EGL_BAD_ALLOC: Not enough resources available.";
                    break;
                case EGL_BAD_CONFIG:
                    errorMessage = U"EGL_BAD_CONFIG: The EGLConfig is not valid.";
                    break;
                default:
                    errorMessage = U"Unknown error: {}"_fmt(error);
                    break;
                }

                LOG_ERROR(U"eglCreateWindowSurface() failed: {}"_fmt(errorMessage));
                throw EngineError(U"eglCreateWindowSurface() failed: {}"_fmt(errorMessage));
            }

            if (eglMakeCurrent(m_display, m_eglSurface, m_eglSurface, m_context) != EGL_TRUE)
            {
                EGLint error = eglGetError();
                LOG_ERROR(U"eglMakeCurrent() failed: {}"_fmt(Unicode::Widen(getEGLErrorString(error))));
                throw EngineError(U"eglMakeCurrent() failed");
            }

            eglSwapInterval(m_display, static_cast<int32>(m_vSyncEnabled));

            EGLint eglError = eglGetError();
            if (eglError != EGL_SUCCESS)
            {
                LOG_ERROR(U"eglInit() failed with error: {}"_fmt(Unicode::Widen(getEGLErrorString(eglError))));
                throw EngineError(U"eglInit() failed");
            }

            const String renderer = Unicode::Widen(reinterpret_cast<const char *>(::glGetString(GL_RENDERER)));
            const String vendor = Unicode::Widen(reinterpret_cast<const char *>(::glGetString(GL_VENDOR)));
            const String version = Unicode::Widen(reinterpret_cast<const char *>(::glGetString(GL_VERSION)));
            const String glslVersion = Unicode::Widen(reinterpret_cast<const char *>(::glGetString(GL_SHADING_LANGUAGE_VERSION)));

            GLint glMajor = 0, glMinor = 0;
            ::glGetIntegerv(GL_MAJOR_VERSION, &glMajor);
            ::glGetIntegerv(GL_MINOR_VERSION, &glMinor);

            GLint maxUniformSize = 0, maxUniformBindings = 0;
            ::glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxUniformSize);
            ::glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxUniformBindings);

            LOG_INFO(U"renderer: {}"_fmt(renderer));
            LOG_INFO(U"vendor: {}"_fmt(vendor));
            LOG_INFO(U"version: {}"_fmt(version));
            LOG_INFO(U"glslVersion: {}"_fmt(glslVersion));
            LOG_INFO(U"GL_MAJOR_VERSION: {}"_fmt(glMajor));
            LOG_INFO(U"GL_MINOR_VERSION: {}"_fmt(glMinor));
            LOG_INFO(U"GL_MAX_UNIFORM_BLOCK_SIZE: {}"_fmt(maxUniformSize));
            LOG_INFO(U"GL_MAX_UNIFORM_BUFFER_BINDINGS: {}"_fmt(maxUniformBindings));

            m_backBuffer = std::make_unique<GLES3BackBuffer>();
            m_blendState = std::make_unique<GLES3BlendState>();
            m_rasterizerState = std::make_unique<GLES3RasterizerState>();
            m_depthStencilState = std::make_unique<GLES3DepthStencilState>();
            m_samplerState = std::make_unique<GLES3SamplerState>();

            pTexture = static_cast<CTexture_GLES3 *>(SIV3D_ENGINE(Texture));
            pTexture->init();

            SIV3D_ENGINE(Shader)->init();
            SIV3D_ENGINE(Mesh)->init();

            clear();
        }
        catch (const std::exception &e)
        {
            LOG_ERROR(U"CRenderer_GLES3::init() failed with exception: {}"_fmt(Unicode::Widen(e.what())));
            throw;
        }
        catch (...)
        {
            LOG_ERROR(U"CRenderer_GLES3::init() failed with unknown exception");
            throw;
        }
    }

    void CRenderer_GLES3::deinit()
    {
        LOG_SCOPED_TRACE(U"CRenderer_GLES3::deinit()");

        try
        {
            if (SIV3D_ENGINE(Mesh))
            {
                SIV3D_ENGINE(Mesh)->deinit();
            }

            if (SIV3D_ENGINE(Shader))
            {
                SIV3D_ENGINE(Shader)->deinit();
            }

            if (pTexture)
            {
                pTexture->deinit();
                pTexture = nullptr;
            }

            m_samplerState.reset();
            m_depthStencilState.reset();
            m_rasterizerState.reset();
            m_blendState.reset();
            m_backBuffer.reset();

            eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

            if (m_display != EGL_NO_DISPLAY)
            {
                if (m_eglSurface != EGL_NO_SURFACE)
                {
                    if (eglDestroySurface(m_display, m_eglSurface) != EGL_TRUE)
                    {
                        EGLint error = eglGetError();
                        LOG_WARNING(U"eglDestroySurface() failed: {}"_fmt(Unicode::Widen(getEGLErrorString(error))));
                    }
                    m_eglSurface = EGL_NO_SURFACE;
                }

                if (m_context != EGL_NO_CONTEXT)
                {
                    if (eglDestroyContext(m_display, m_context) != EGL_TRUE)
                    {
                        EGLint error = eglGetError();
                        LOG_WARNING(U"eglDestroyContext() failed: {}"_fmt(Unicode::Widen(getEGLErrorString(error))));
                    }
                    m_context = EGL_NO_CONTEXT;
                }

                if (eglTerminate(m_display) != EGL_TRUE)
                {
                    EGLint error = eglGetError();
                    LOG_WARNING(U"eglTerminate() failed: {}"_fmt(Unicode::Widen(getEGLErrorString(error))));
                }

                m_display = EGL_NO_DISPLAY;
            }

            pRenderer2D = nullptr;
            pRenderer3D = nullptr;
            m_window = nullptr;
            m_surfaceValid = false;
            m_config = nullptr;
        }

        catch (const std::exception &e)
        {
            LOG_ERROR(U"CRenderer_GLES3::deinit() failed with exception: {}"_fmt(Unicode::Widen(e.what())));
        }
        catch (...)
        {
            LOG_ERROR(U"CRenderer_GLES3::deinit() failed with unknown exception");
        }
    }

    StringView CRenderer_GLES3::getName() const
    {
        static constexpr StringView name(U"WebGL2");
        return name;
    }

    void CRenderer_GLES3::clear()
    {
        m_backBuffer->clear(GLES3ClearTarget::Scene);

        const auto &windowState = SIV3D_ENGINE(Window)->getState();

        if (const Size frameBufferSize = windowState.frameBufferSize;
            (frameBufferSize != m_backBuffer->getBackBufferSize()))
        {
            m_backBuffer->setBackBufferSize(frameBufferSize);

            if (windowState.sizeMove)
            {
                // sleep
            }
        }
    }

    void CRenderer_GLES3::flush()
    {
        pRenderer3D->flush();
        pRenderer2D->flush();
        m_backBuffer->unbind();
        m_backBuffer->updateFromSceneBuffer();
    }

    bool CRenderer_GLES3::present()
    {
        eglSwapBuffers(eglGetCurrentDisplay(), eglGetCurrentSurface(EGL_DRAW));

        if constexpr (SIV3D_BUILD(DEBUG))
        {
            CheckOpenGLESError();
        }

        return true;
    }

    void CRenderer_GLES3::setVSyncEnabled(const bool enabled)
    {
        if (m_vSyncEnabled == enabled)
        {
            return;
        }

        m_vSyncEnabled = enabled;

        eglSwapInterval(m_display, static_cast<int32>(m_vSyncEnabled));
    }

    bool CRenderer_GLES3::isVSyncEnabled() const
    {
        return m_vSyncEnabled;
    }

    void CRenderer_GLES3::captureScreenshot()
    {
        m_backBuffer->capture();
    }

    const Image &CRenderer_GLES3::getScreenCapture() const
    {
        return m_backBuffer->getScreenCapture();
    }

    void CRenderer_GLES3::setSceneResizeMode(const ResizeMode resizeMode)
    {
        m_backBuffer->setSceneResizeMode(resizeMode);
    }

    ResizeMode CRenderer_GLES3::getSceneResizeMode() const noexcept
    {
        return m_backBuffer->getSceneResizeMode();
    }

    void CRenderer_GLES3::setSceneBufferSize(const Size size)
    {
        m_backBuffer->setSceneBufferSize(size);
    }

    Size CRenderer_GLES3::getSceneBufferSize() const noexcept
    {
        return m_backBuffer->getSceneBufferSize();
    }

    void CRenderer_GLES3::setSceneTextureFilter(const TextureFilter textureFilter)
    {
        m_backBuffer->setSceneTextureFilter(textureFilter);
    }

    TextureFilter CRenderer_GLES3::getSceneTextureFilter() const noexcept
    {
        return m_backBuffer->getSceneTextureFilter();
    }

    void CRenderer_GLES3::setBackgroundColor(const ColorF &color)
    {
        m_backBuffer->setBackgroundColor(color);
    }

    const ColorF &CRenderer_GLES3::getBackgroundColor() const noexcept
    {
        return m_backBuffer->getBackgroundColor();
    }

    void CRenderer_GLES3::setLetterboxColor(const ColorF &color)
    {
        m_backBuffer->setLetterboxColor(color);
    }

    const ColorF &CRenderer_GLES3::getLetterboxColor() const noexcept
    {
        return m_backBuffer->getLetterBoxColor();
    }

    std::pair<float, RectF> CRenderer_GLES3::getLetterboxComposition() const noexcept
    {
        return m_backBuffer->getLetterboxComposition();
    }

    void CRenderer_GLES3::updateSceneSize()
    {
        m_backBuffer->updateSceneSize();
    }

    GLES3BackBuffer &CRenderer_GLES3::getBackBuffer() noexcept
    {
        return *m_backBuffer;
    }

    GLES3BlendState &CRenderer_GLES3::getBlendState() noexcept
    {
        return *m_blendState;
    }

    GLES3RasterizerState &CRenderer_GLES3::getRasterizerState() noexcept
    {
        return *m_rasterizerState;
    }

    GLES3DepthStencilState &CRenderer_GLES3::getDepthStencilState() noexcept
    {
        return *m_depthStencilState;
    }

    GLES3SamplerState &CRenderer_GLES3::getSamplerState() noexcept
    {
        return *m_samplerState;
    }
}
