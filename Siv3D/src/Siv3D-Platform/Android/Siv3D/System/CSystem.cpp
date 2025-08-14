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

#include <Siv3D/EngineLog.hpp>
#include <Siv3D/Unicode.hpp>
#include <Siv3D/Common/Siv3DEngine.hpp>
#include <Siv3D/Resource/IResource.hpp>
#include <Siv3D/Profiler/IProfiler.hpp>
#include <Siv3D/AssetMonitor/IAssetMonitor.hpp>
#include <Siv3D/LicenseManager/ILicenseManager.hpp>
#include <Siv3D/ImageDecoder/IImageDecoder.hpp>
#include <Siv3D/ImageEncoder/IImageEncoder.hpp>
#include <Siv3D/Window/IWindow.hpp>
#include <Siv3D/Scene/IScene.hpp>
#include <Siv3D/Cursor/ICursor.hpp>
#include <Siv3D/Keyboard/IKeyboard.hpp>
#include <Siv3D/Mouse/IMouse.hpp>
#include <Siv3D/XInput/IXInput.hpp>
#include <Siv3D/Gamepad/IGamepad.hpp>
#include <Siv3D/Pentablet/IPentablet.hpp>
#include <Siv3D/TextInput/ITextInput.hpp>
#include <Siv3D/Clipboard/IClipboard.hpp>
#include <Siv3D/DragDrop/IDragDrop.hpp>
#include <Siv3D/ToastNotification/IToastNotification.hpp>
#include <Siv3D/Network/INetwork.hpp>
#include <Siv3D/SoundFont/ISoundFont.hpp>
#include <Siv3D/AudioCodec/IAudioCodec.hpp>
#include <Siv3D/AudioDecoder/IAudioDecoder.hpp>
#include <Siv3D/AudioEncoder/IAudioEncoder.hpp>
#include <Siv3D/FFT/IFFT.hpp>
#include <Siv3D/Audio/IAudio.hpp>
#include <Siv3D/TextToSpeech/ITextToSpeech.hpp>
#include <Siv3D/Renderer/IRenderer.hpp>
#include <Siv3D/Renderer2D/IRenderer2D.hpp>
#include <Siv3D/Renderer3D/IRenderer3D.hpp>
#include <Siv3D/ScreenCapture/IScreenCapture.hpp>
#include <Siv3D/Model/IModel.hpp>
#include <Siv3D/UserAction/IUserAction.hpp>
#include <Siv3D/Font/IFont.hpp>
#include <Siv3D/GUI/IGUI.hpp>
#include <Siv3D/Print/IPrint.hpp>
#include <Siv3D/PrimitiveMesh/IPrimitiveMesh.hpp>
#include <Siv3D/Asset/IAsset.hpp>
#include <Siv3D/Effect/IEffect.hpp>
#include <Siv3D/Script/IScript.hpp>
#include <Siv3D/Addon/IAddon.hpp>
#include <Siv3D/System/SystemLog.hpp>
#include <Siv3D/System/SystemMisc.hpp>
#include <Siv3D/Shader/IShader.hpp>
#include "CSystem.hpp"

extern bool g_isRenderingSuspended;
extern bool g_isSuspending;
extern bool g_isResuming;
extern std::mutex g_CallbackMutex;

void OnSuspend();
void OnResume();

namespace s3d
{
    CSystem::CSystem()
    {
    }

    CSystem::~CSystem()
    {
        LOG_SCOPED_TRACE(U"CSystem::~CSystem()");

        SystemMisc::Destroy();
        SystemLog::Final();
    }

    void CSystem::init()
    {
        SystemLog::Initial();

        LOG_SCOPED_TRACE(U"CSystem::init()");

        try
        {
            SystemMisc::Init();

            SIV3D_ENGINE(Resource)->init();
            SIV3D_ENGINE(Profiler)->init();
            SIV3D_ENGINE(Window)->init();
            SIV3D_ENGINE(ImageDecoder)->init();
            SIV3D_ENGINE(ImageEncoder)->init();
            SIV3D_ENGINE(Cursor)->init();
            SIV3D_ENGINE(Keyboard)->init();
            SIV3D_ENGINE(Mouse)->init();
            SIV3D_ENGINE(XInput)->init();
            SIV3D_ENGINE(Gamepad)->init();
            SIV3D_ENGINE(Pentablet)->init();
            SIV3D_ENGINE(TextInput)->init();
            SIV3D_ENGINE(Clipboard)->init();
            SIV3D_ENGINE(DragDrop)->init();
            SIV3D_ENGINE(ToastNotification)->init();
            SIV3D_ENGINE(SoundFont)->init();
            SIV3D_ENGINE(AudioCodec)->init();
            SIV3D_ENGINE(AudioDecoder)->init();
            SIV3D_ENGINE(AudioEncoder)->init();
            SIV3D_ENGINE(FFT)->init();
            SIV3D_ENGINE(Audio)->init();
            SIV3D_ENGINE(TextToSpeech)->init();

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

            LOG_INFO(U"CSystem::init() completed successfully");
        }
        catch (const std::exception &e)
        {
            LOG_ERROR(U"Exception during CSystem::init(): {}"_fmt(Unicode::Widen(e.what())));
            throw;
        }
        catch (...)
        {
            LOG_ERROR(U"Unknown exception during CSystem::init()");
            throw;
        }
    }

    bool CSystem::update()
    {
        while (g_isRenderingSuspended)
        {
            if (m_termination || SIV3D_ENGINE(UserAction)->terminationTriggered())
            {
                m_termination = true;
            }

            if (m_termination)
            {
                return false;
            }

            {
                std::lock_guard<std::mutex> lock(g_CallbackMutex);
                if (g_isResuming)
                {
                    LOG_INFO(U"CSystem::update() - Detected resume flag, calling OnResume()");
                    OnResume();
                    if (SIV3D_ENGINE(UserAction)->terminationTriggered())
                    {
                        return false;
                    }

                    g_isResuming = false;
                    g_isRenderingSuspended = false;
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        try
        {
            SIV3D_ENGINE(Addon)->draw();
            SIV3D_ENGINE(Print)->draw();
            SIV3D_ENGINE(Renderer)->flush();
            SIV3D_ENGINE(Profiler)->endFrame();
            SIV3D_ENGINE(Renderer)->present();
            SIV3D_ENGINE(ScreenCapture)->update();

            //
            // previous frame
            //
            // -----------------------------------
            //
            // current frame
            //

            SIV3D_ENGINE(Profiler)->beginFrame();
            if (not SIV3D_ENGINE(AssetMonitor)->update())
            {
                m_termination = true;
                return false;
            }
            SIV3D_ENGINE(Scene)->update();
            SIV3D_ENGINE(Window)->update();
            SIV3D_ENGINE(Renderer)->clear();
            SIV3D_ENGINE(Asset)->update();
            SIV3D_ENGINE(Cursor)->update();
            SIV3D_ENGINE(Keyboard)->update();
            SIV3D_ENGINE(Mouse)->update();
            SIV3D_ENGINE(XInput)->update(false);
            SIV3D_ENGINE(Gamepad)->update();
            SIV3D_ENGINE(Pentablet)->update();
            SIV3D_ENGINE(TextInput)->update();
            SIV3D_ENGINE(DragDrop)->update();
            SIV3D_ENGINE(Effect)->update();
            if (not SIV3D_ENGINE(Addon)->update())
            {
                m_termination = true;
                return false;
            }

            SIV3D_ENGINE(LicenseManager)->update();

            return true;
        }
        catch (const std::exception &e)
        {
            LOG_ERROR(U"Exception during CSystem::update(): {}"_fmt(Unicode::Widen(e.what())));
            return false;
        }
        catch (...)
        {
            LOG_ERROR(U"Unknown exception during CSystem::update()");
            return false;
        }
    }

}
