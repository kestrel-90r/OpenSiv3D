//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2025 kestrel-90r
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

#include <jni.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <vector>
#include <android/keycodes.h>
#include <algorithm>
#include <atomic>
#include <memory>
#include <pthread.h>

#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include <camera/NdkCameraManager.h>
#include <media/NdkImageReader.h>
#include <camera/NdkCameraMetadataTags.h>

#include <Siv3D.hpp>
#include <Siv3D/Common/Siv3DEngine.hpp>
#include <Siv3D/EngineLog.hpp>
#include <Siv3D/Window/CWindow.hpp>
#include <Siv3D/Mouse/CMouse.hpp>
#include <Siv3D/Cursor/CCursor.hpp>
#include <Siv3D/Keyboard/CKeyboard.hpp>
#include <Siv3D/TextInput/ITextInput.hpp>
#include <Siv3D/TextInput/CTextInput.hpp>
#include <Siv3D/Renderer/GLES3/CRenderer_GLES3.hpp>

#include "AGDK/GameActivity.h"
#include "AGDK/game-text-input/gametextinput.h"

#define TAG "Siv3D"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

bool isSiv3DRunning();
void StartSuspend();
void StartResume();

JavaVM *g_JavaVM = nullptr;
static pthread_t g_siv3d_thread;
static bool g_siv3d_thread_started = false;

jobject g_mainActivityInstance = nullptr;

int32_t g_reinitAttempts = 0;

static void start_siv3d_thread();

extern "C" void *siv3d_main_thread(void *arg);
extern "C" int Siv3DMain(int argc, char *argv[], int width, int height);
extern bool g_isRenderingSuspended;

namespace s3d
{
    AAssetManager *g_AssetManager = nullptr;
    ANativeWindow *g_NativeWindow = nullptr;
    extern void *g_GameActivityHandle;

    using InitializerFunc = void (*)(void *);

    static InitializerFunc g_initializerFunc = nullptr;
    static void *g_initializerData = nullptr;

    void Initializer(InitializerFunc func, void *userData = nullptr)
    {
        g_initializerFunc = func;
        g_initializerData = userData;
        LOGI("App initializer function registered");

        if (g_initializerFunc)
        {
            g_initializerFunc(g_initializerData);
            LOGI("Initial initialization executed");
        }
    }
}

void routeTouchEventToSiv3D(int action, int x, int y)
{
    using namespace s3d;

    if (auto *cursor = dynamic_cast<CCursor *>(SIV3D_ENGINE(Cursor)))
    {
        cursor->onTouchEvent(Point{x, y});
    }

    if (auto *mouse = dynamic_cast<CMouse *>(SIV3D_ENGINE(Mouse)))
    {
        mouse->onTouchEvent(action, Point{x, y});
    }
}

/// フレームバッファサイズを Kotlinから通知
/// @param env JNI 環境
/// @param thiz MainActivity
/// @param width 幅
/// @param height 高さ
extern "C" JNIEXPORT void JNICALL
Java_com_kestrel_opensiv3d_MainActivity_SendFrameBufferSizeNative(JNIEnv *env, jobject thiz, jint width, jint height)
{
    LOG_TRACE(U"Java_com_kestrel_opensiv3d_MainActivity_SendFrameBufferSizeNative called {} x {}"_fmt(width, height));

    if (isSiv3DRunning())
    {
        try
        {
            bool result = SIV3D_ENGINE(Window)->resizeByFrameBufferSize(s3d::Size{width, height});
            if (result)
            {
                LOGI("Notified Siv3D engine of size change: %dx%d", width, height);
            }
            else
            {
                LOGE("Failed to resize frame buffer: %dx%d", width, height);
            }
        }
        catch (const std::exception &e)
        {
            LOGE("Failed to notify Siv3D engine of frame buffer size change: %s", e.what());
        }
    }
    else
    {
        LOG_TRACE(U"Siv3D engine not running, frame buffer size change will be applied when engine starts");
    }
}

/// 単一タッチイベントを Siv3D の Cursor / Mouse へ転送
/// @param env JNI 環境
/// @param thiz MainActivity
/// @param action MotionEvent アクション
/// @param x X 座標（ピクセル）
/// @param y Y 座標（ピクセル）
extern "C" JNIEXPORT void JNICALL
Java_com_kestrel_opensiv3d_MainActivity_onTouchEventNative(JNIEnv *env, jobject /* this */, jint action, jint x, jint y)
{
    auto *cursor = dynamic_cast<CCursor *>(SIV3D_ENGINE(Cursor));
    auto *mouse = dynamic_cast<CMouse *>(SIV3D_ENGINE(Mouse));

    if (cursor)
        cursor->onTouchEvent(Point{static_cast<int>(x), static_cast<int>(y)});

    if (mouse)
        mouse->onTouchEvent(action, Point{static_cast<int>(x), static_cast<int>(y)});
}

/// Activity 再開時にアプリの描画/更新をリジューム開始
/// @param env JNI 環境
/// @param obj GameActivity
extern "C" JNIEXPORT void JNICALL
Java_com_google_androidgamesdk_GameActivity_onResumeNative(JNIEnv *env, jobject obj, jlong handle)
{
    LOGI("onResumeNative - Resuming application");
    StartResume();
}


/// Activity 一時停止時に描画停止しサスペンド開始
/// @param env JNI 環境
/// @param obj GameActivity
extern "C" JNIEXPORT void JNICALL
Java_com_google_androidgamesdk_GameActivity_onPauseNative(JNIEnv *env, jobject obj, jlong handle)
{
    LOGI("onPauseNative - Suspending surface");

    StartSuspend();
    LOGI("Suspend requested");
}

extern "C" void updateNativeWindowHandle(void *windowHandle)
{
    if (windowHandle)
    {
        s3d::g_NativeWindow = static_cast<ANativeWindow *>(windowHandle);
        s3d::g_GameActivityHandle = windowHandle;
        LOGI("Native window handle updated directly: %p", windowHandle);
    }

    else if (s3d::g_NativeWindow)
    {
        LOGI("Using existing native window: %p", s3d::g_NativeWindow);
        s3d::g_GameActivityHandle = s3d::g_NativeWindow;
    }
    else
    {
        LOGI("Warning: updateNativeWindowHandle - No valid handle available");
    }
}

/// DPAD と ENTERのキー押下取得→Siv3D の Keyboard へ転送
/// @param env JNI 環境
/// @param thiz MainActivity
/// @param keyCode Android の AKEYCODE_*
/// @return 消費した場合 JNI_TRUE、未消費は JNI_FALSE
extern "C" JNIEXPORT jboolean JNICALL
Java_com_kestrel_opensiv3d_MainActivity_onKeyDownNative(JNIEnv *env, jobject thiz, jint keyCode)
{
    auto keyboard = dynamic_cast<CKeyboard *>(SIV3D_ENGINE(Keyboard));

    if (keyboard)
    {
        keyboard->onKeyEvent(keyCode, true);

        if (keyCode == AKEYCODE_DPAD_UP || keyCode == AKEYCODE_DPAD_DOWN ||
            keyCode == AKEYCODE_DPAD_LEFT || keyCode == AKEYCODE_DPAD_RIGHT || keyCode == AKEYCODE_ENTER)
        {
            return JNI_TRUE;
        }
    }
    return JNI_FALSE;
}

/// 物理キーボードのキー解放取得→Siv3D の Keyboard へ転送
/// @param env JNI 環境
/// @param thiz MainActivity
/// @param keyCode Android の AKEYCODE_*
/// @return 常に JNI_TRUE
extern "C" JNIEXPORT jboolean JNICALL
Java_com_kestrel_opensiv3d_MainActivity_onKeyUpNative(JNIEnv *env, jobject thiz, jint keyCode)
{
    auto keyboard = dynamic_cast<CKeyboard *>(SIV3D_ENGINE(Keyboard));
    keyboard->onKeyEvent(keyCode, false);
    return JNI_TRUE;
}

/// Kotlinの文字入力（UTF-8）→Siv3D の TextInput へ転送
/// @param env JNI 環境
/// @param thiz MainActivity
/// @param text 入力文字列（UTF-8）
/// @return 成功で JNI_TRUE、失敗で JNI_FALSE
extern "C" JNIEXPORT jboolean JNICALL
Java_com_kestrel_opensiv3d_MainActivity_onTextInputNative(JNIEnv *env, jobject thiz, jstring text)
{
    if (!text) return JNI_FALSE;

    const char *utfText = env->GetStringUTFChars(text, nullptr);
    if (!utfText) return JNI_FALSE;

    const String s = Unicode::FromUTF8(utfText);

    auto textInput = SIV3D_ENGINE(TextInput);
    for (const auto &ch : s)
    {
        textInput->pushChar(ch);
    }

    env->ReleaseStringUTFChars(text, utfText);
    return JNI_TRUE;
}

/// Activity の onCreate初期化処理
/// @param env JNI 環境
/// @param thiz MainActivity
/// @param assetManager Java の AssetManager
/// @param internalDataPath 内部ストレージパス
/// @param externalDataPath 外部ストレージパス
/// @param packageName パッケージ名
/// @return 成功で JNI_TRUE
extern "C" JNIEXPORT jboolean JNICALL
Java_com_kestrel_opensiv3d_MainActivity_onCreateNative(JNIEnv *env, jobject thiz,
         jobject assetManager, jstring internalDataPath, jstring externalDataPath, jstring packageName)
{
    LOG_TRACE(U"onCreateNative called");

    if (assetManager)
    {
        jobject globalAssetManager = env->NewGlobalRef(assetManager);
        AAssetManager *nativeAssetManager = AAssetManager_fromJava(env, globalAssetManager);

        s3d::g_AssetManager = nativeAssetManager;
    }

    if (internalDataPath)
    {
        const char *path = env->GetStringUTFChars(internalDataPath, nullptr);
        env->ReleaseStringUTFChars(internalDataPath, path);
    }

    if (externalDataPath)
    {
        const char *path = env->GetStringUTFChars(externalDataPath, nullptr);
        env->ReleaseStringUTFChars(externalDataPath, path);
    }

    if (packageName)
    {
        const char *name = env->GetStringUTFChars(packageName, nullptr);
        env->ReleaseStringUTFChars(packageName, name);
    }

    return JNI_TRUE;
}

/// IME のフォーカス状態切り替え通知
/// @param env JNI 環境
/// @param thiz MainActivity
/// @param focused true で有効化、false で無効化
/// @return 常に JNI_TRUE
extern "C" JNIEXPORT jboolean JNICALL
Java_com_kestrel_opensiv3d_MainActivity_onTextInputFocusNative(JNIEnv *env, jobject thiz, jboolean focused)
{
    LOG_TRACE(U"Text Input Focus: {}"_fmt(focused ? U"true" : U"false"));

    auto textInput = SIV3D_ENGINE(TextInput);

    if (focused)
    {
        textInput->enableIME(true);
    }
    else
    {
        textInput->enableIME(false);
    }

    return JNI_TRUE;
}

/// Activity/GL/レンダリング状態保存データ
struct SavedState
{
    int32_t windowWidth = 0;
    int32_t windowHeight = 0;
    bool engineInitialized = false;
    bool eglNeedsReinit = false;
    bool renderingSuspended = false;
    int32_t reinitAttempts = 0;
    GLint viewport[4] = {0, 0, 0, 0};
    bool depthTestEnabled = false;
    bool blendEnabled = false;
};

/// 現在のテキスト入力内容をUTF-8文字列で取得
/// @param env JNI 環境
/// @param thiz MainActivity
/// @return 現在のテキスト（UTF-8）
extern "C" JNIEXPORT jstring JNICALL
Java_com_kestrel_opensiv3d_MainActivity_nativeGetCurrentText(JNIEnv *env, jobject thiz)
{
    auto textInput = dynamic_cast<CTextInput *>(SIV3D_ENGINE(TextInput));
    if (!textInput)
    {
        return env->NewStringUTF("");
    }

    const String &text = textInput->getCurrentText();
    const std::string utf8 = Unicode::ToUTF8(text);
    return env->NewStringUTF(utf8.c_str());
}

/// Siv3D のメインスレッド
/// @param arg 未使用
/// @return 未使用
extern "C" void *siv3d_main_thread(void *arg)
{
    int32_t width = ANativeWindow_getWidth(s3d::g_NativeWindow);
    int32_t height = ANativeWindow_getHeight(s3d::g_NativeWindow);

    LOGI("Starting Siv3DMain with surface: %p (%dx%d)", s3d::g_NativeWindow, width, height);

    const char *defaultPath = "/android/app";
    char *defaultArgv[] = {const_cast<char *>(defaultPath), nullptr};

    g_siv3d_thread_started = true;

    int ret = Siv3DMain(1, defaultArgv, width, height);
    if (ret != 0)
    {
        LOGE("Siv3DMain returned error: %d", ret);
    }

    LOGI("Siv3DMain thread ending normally");
    g_siv3d_thread_started = false;
    return NULL;
}

/// Siv3D メインスレッドを非結合スレッドとして起動
static void start_siv3d_thread()
{
    if (!g_siv3d_thread_started)
    {
        LOGI("Creating Siv3D main thread");
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&g_siv3d_thread, &attr, siv3d_main_thread, nullptr);
        pthread_attr_destroy(&attr);
    }
}

/// GameActivity からの単一Unicode 入力を CTextInput に転送
/// @param env JNI 環境
/// @param clazz GameActivity クラス
/// @param unicodeChar 入力された Unicode スカラ値
extern "C" JNIEXPORT void JNICALL
Java_com_kestrel_opensiv3d_MainActivity_nativeOnTextInput(
    JNIEnv *env, jclass clazz, jint unicodeChar)
{
    if (auto *textInput = static_cast<s3d::CTextInput *>(s3d::SIV3D_ENGINE(TextInput)))
    {
        textInput->pushChar(static_cast<uint32_t>(unicodeChar));
    }
    else
    {
        LOGE("CTextInput engine not available!");
    }
}

/// ファイル選択ダイアログの結果を Kotlin から受け取るコールバック
/// スレッドセーフに結果を保持し、待機中のスレッドへ通知
/// @param env JNI 環境
/// @param obj MainActivity
/// @param filePath 選択ファイルのパス（null ならキャンセル）
static std::string g_selectedFilePath;
static std::mutex g_fileSelectionMutex;
static std::condition_variable g_fileSelectionCV;
static bool g_fileSelectionComplete = false;

extern "C" JNIEXPORT void JNICALL
Java_com_kestrel_opensiv3d_MainActivity_onFileSelectedNative(JNIEnv *env, jobject obj, jstring filePath)
{
    std::lock_guard<std::mutex> lock(g_fileSelectionMutex);

    if (filePath)
    {
        const char *path = env->GetStringUTFChars(filePath, nullptr);
        g_selectedFilePath = path;
        env->ReleaseStringUTFChars(filePath, path);
    }
    else
    {
        g_selectedFilePath = "";
    }

    g_fileSelectionComplete = true;
    g_fileSelectionCV.notify_one();
}

/// Kotlin の startFileSelectionDialog() を呼び出し、ダイアログを起動
/// 起動前にアプリをサスペンド
/// @return 起動に成功すれば true
bool openFileSelectionDialog()
{
    LOGI("File selection dialog starting - suspending application");
    StartSuspend();

    if (!g_JavaVM || !g_mainActivityInstance)
    {
        return false;
    }

    JNIEnv *env;
    if (g_JavaVM->AttachCurrentThread(&env, nullptr) != JNI_OK)
    {
        return false;
    }

    jclass activityClass = env->GetObjectClass(g_mainActivityInstance);
    if (!activityClass)
    {
        g_JavaVM->DetachCurrentThread();
        return false;
    }

    jmethodID startFileSelectionMethod = env->GetMethodID(activityClass, "startFileSelectionDialog", "()Z");
    if (!startFileSelectionMethod)
    {
        env->DeleteLocalRef(activityClass);
        g_JavaVM->DetachCurrentThread();
        return false;
    }

    jboolean result = env->CallBooleanMethod(g_mainActivityInstance, startFileSelectionMethod);

    env->DeleteLocalRef(activityClass);
    g_JavaVM->DetachCurrentThread();

    if (result == JNI_TRUE)
        return true;
    else
        return false;
}

/// ファイル選択の完了待機し、選択結果を返す（ブロッキング処理）
/// @return 選択されたファイルパス。キャンセル時は空文字。
std::string waitForFileSelection()
{
    std::unique_lock<std::mutex> lock(g_fileSelectionMutex);
    g_fileSelectionCV.wait(lock, []
                           { return g_fileSelectionComplete; });

    std::string result = g_selectedFilePath;
    g_fileSelectionComplete = false;
    g_selectedFilePath.clear();
    return result;
}

/// ファイル選択用に MainActivity のグローバル参照と JavaVM を設定
/// 既存の参照があれば解放して差し替える。
/// @param env JNI 環境
/// @param obj MainActivity
extern "C" JNIEXPORT void JNICALL
Java_com_kestrel_opensiv3d_MainActivity_setMainActivityForFileSelection(JNIEnv *env, jobject obj)
{
    if (g_mainActivityInstance)
    {
        env->DeleteGlobalRef(g_mainActivityInstance);
    }

    g_mainActivityInstance = env->NewGlobalRef(obj);

    env->GetJavaVM(&g_JavaVM);
}

namespace CameraSystem
{
    struct CameraFrame
    {
        Image image;
        bool isNew = false;
        int64_t timestamp = 0;
    };

    static std::mutex g_cameraMutex;
    static CameraFrame g_cameraFrame;

    /// YUV_420→RGB変換および必要に応じて画像リサイズ
    /// @param yPlane Y 面先頭ポインタ
    /// @param uPlane U 面先頭ポインタ
    /// @param vPlane V 面先頭ポインタ
    /// @param yRowStride Y 面の行ストライド
    /// @param uRowStride U 面の行ストライド
    /// @param vRowStride V 面の行ストライド
    /// @param uvPixelStride UV のピクセルストライド（1 or 2）
    /// @param width 画像幅
    /// @param height 画像高
    /// @param outImage 変換先 Image
    static void convertYUV420ToRGB(const uint8 *yPlane, const uint8 *uPlane, const uint8 *vPlane,
                                   int yRowStride, int uRowStride, int vRowStride, int uvPixelStride,
                                   int width, int height, Image &outImage)
    {
        static bool tablesInitialized = false;
        static int yTable[256];
        static int uTableR[256], uTableG[256], uTableB[256];
        static int vTableR[256], vTableG[256], vTableB[256];

        if (!tablesInitialized)
        {
            for (int i = 0; i < 256; ++i)
            {
                yTable[i] = i;
                int u = i - 128;
                int v = i - 128;
                uTableR[i] = 0;
                uTableG[i] = static_cast<int>(-0.344 * u);
                uTableB[i] = static_cast<int>(1.772 * u);
                vTableR[i] = static_cast<int>(1.402 * v);
                vTableG[i] = static_cast<int>(-0.714 * v);
                vTableB[i] = 0;
            }
            tablesInitialized = true;
        }

        if (outImage.size() != Size(width, height))
        {
            outImage.resize(width, height);
        }

        size_t yLen = yRowStride * height;
        size_t uLen = uRowStride * ((height + 1) / 2);
        size_t vLen = vRowStride * ((height + 1) / 2);

        for (int y = 0; y < height; ++y)
        {
            Color *row = &outImage[y][0];
            const uint8 *yRow = yPlane + y * yRowStride;

            const int uvY = y / 2;
            const uint8 *uRow = uPlane + uvY * uRowStride;
            const uint8 *vRow = vPlane + uvY * vRowStride;

            for (int x = 0; x < width; ++x)
            {
                const int yVal = yTable[yRow[x]];
                const int uvX = (x / 2) * uvPixelStride;
                const int uVal = uRow[uvX];
                const int vVal = vRow[uvX];

                const int r = yVal + vTableR[vVal];
                const int g = yVal + uTableG[uVal] + vTableG[vVal];
                const int b = yVal + uTableB[uVal];

                row[x] = Color(
                    static_cast<uint8>(Clamp(r, 0, 255)),
                    static_cast<uint8>(Clamp(g, 0, 255)),
                    static_cast<uint8>(Clamp(b, 0, 255)),
                    static_cast<uint8>(255));
            }
        }
    }

    /// Javaの画像ByteBufferのYUV→RGB変換し最新フレームを格納
    /// @param env JNI 環境
    /// @param obj 呼出元オブジェクト（未使用）
    /// @param yBuffer Y 面の DirectByteBuffer
    /// @param uBuffer U 面の DirectByteBuffer
    /// @param vBuffer V 面の DirectByteBuffer
    /// @param yRowStride Y 面行ストライド
    /// @param uvRowStride U/V 行ストライド
    /// @param uvPixelStride U/V ピクセルストライド
    /// @param width 幅
    /// @param height 高さ
    static void onYuvFrameRaw(const uint8 *yPlane, const uint8 *uPlane, const uint8 *vPlane,
                              int yRowStride, int uRowStride, int vRowStride, int uvPixelStride,
                              int width, int height)
    {
        std::lock_guard<std::mutex> lock(g_cameraMutex);
        convertYUV420ToRGB(yPlane, uPlane, vPlane,
                           yRowStride, uRowStride, vRowStride, uvPixelStride,
                           width, height, g_cameraFrame.image);
        g_cameraFrame.isNew = true;
        g_cameraFrame.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                                      std::chrono::steady_clock::now().time_since_epoch())
                                      .count();
    }

    /// Javaの画像ByteBufferのYUV→RGB変換し最新フレームを格納
    /// @param env JNI 環境
    /// @param obj 呼出元オブジェクト（未使用）
    /// @param yBuffer Y 面の DirectByteBuffer
    /// @param uBuffer U 面の DirectByteBuffer
    /// @param vBuffer V 面の DirectByteBuffer
    /// @param yRowStride Y 面行ストライド
    /// @param uvRowStride U/V 行ストライド
    /// @param uvPixelStride U/V ピクセルストライド
    /// @param width 幅
    /// @param height 高さ
    void onCameraFrame(JNIEnv *env, jobject obj,
                       jobject yBuffer, jobject uBuffer, jobject vBuffer,
                       jint yRowStride, jint uvRowStride, jint uvPixelStride,
                       jint width, jint height)
    {
        const uint8 *yPlane = static_cast<const uint8 *>(env->GetDirectBufferAddress(yBuffer));
        const uint8 *uPlane = static_cast<const uint8 *>(env->GetDirectBufferAddress(uBuffer));
        const uint8 *vPlane = static_cast<const uint8 *>(env->GetDirectBufferAddress(vBuffer));

        if (!yPlane || !uPlane || !vPlane)
        {
            LOGE("Failed to get direct buffer addresses");
            return;
        }

        jlong yBufferSize = env->GetDirectBufferCapacity(yBuffer);
        jlong uBufferSize = env->GetDirectBufferCapacity(uBuffer);
        jlong vBufferSize = env->GetDirectBufferCapacity(vBuffer);

        if (yBufferSize < yRowStride * height ||
            uBufferSize + 1 < uvRowStride * (height / 2) ||
            vBufferSize + 1 < uvRowStride * (height / 2))
        {
            LOGE("Buffer size insufficient: Y=%ld, U=%ld, V=%ld", yBufferSize, uBufferSize, vBufferSize);
            return;
        }

        {
            std::lock_guard<std::mutex> lock(g_cameraMutex);

            convertYUV420ToRGB(yPlane, uPlane, vPlane,
                               yRowStride, uvRowStride, uvRowStride, uvPixelStride,
                               width, height, g_cameraFrame.image);

            g_cameraFrame.isNew = true;
            g_cameraFrame.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                                          std::chrono::steady_clock::now().time_since_epoch())
                                          .count();
        }
    }

    /// 新しいカメラフレームの有無
    /// @return 新規フレームがあれば true
    bool hasNewFrame()
    {
        std::lock_guard<std::mutex> lock(g_cameraMutex);
        return g_cameraFrame.isNew;
    }

    /// 最新カメラフレーム取得
    /// @return 画像（新規なしの場合は空）
    Image getLatestFrame()
    {
        std::lock_guard<std::mutex> lock(g_cameraMutex);
        if (g_cameraFrame.isNew)
        {
            g_cameraFrame.isNew = false;
            return g_cameraFrame.image;
        }
        return Image{};
    }

    /// 直近フレームのタイムスタンプ取得
    /// @return 時刻（ミリ秒）
    int64_t getLastFrameTimestamp()
    {
        std::lock_guard<std::mutex> lock(g_cameraMutex);
        return g_cameraFrame.timestamp;
    }
}

/// MainActivityからのカメラフレーム取得→CameraSystem::onCameraFrame へ転送
extern "C" JNIEXPORT void JNICALL
Java_com_kestrel_opensiv3d_MainActivity_onCameraFrame(JNIEnv *env, jobject obj,
                                                      jobject yBuffer, jobject uBuffer, jobject vBuffer, jint yRowStride, jint uvRowStride,
                                                      jint uvPixelStride, jint width, jint height)
{
    CameraSystem::onCameraFrame(env, obj, yBuffer, uBuffer, vBuffer,
                                yRowStride, uvRowStride, uvPixelStride, width, height);
}

namespace
{
    ACameraManager *g_camManager = nullptr;
    ACameraDevice *g_camDevice = nullptr;
    ACaptureSessionOutput *g_sessionOutput = nullptr;
    ACaptureSessionOutputContainer *g_outputContainer = nullptr;
    ACameraOutputTarget *g_outputTarget = nullptr;
    ACaptureRequest *g_request = nullptr;
    ACameraCaptureSession *g_session = nullptr;
    AImageReader *g_imageReader = nullptr;
    ANativeWindow *g_readerWindow = nullptr;
    std::atomic<bool> g_cameraActive{false};

    /// CaptureSession アクティブ通知
    /// @param ctx 未使用
    /// @param session キャプチャセッション
    void onSessionActive(void * /*ctx*/, ACameraCaptureSession * /*session*/)
    {
        LOGI("NDK Camera session is active");
        g_cameraActive.store(true);
    }

    /// CaptureSession クローズ通知
    /// @param ctx 未使用
    /// @param session キャプチャセッション
    void onSessionClosed(void * /*ctx*/, ACameraCaptureSession * /*session*/)
    {
        LOGI("NDK Camera session is closed");
        g_cameraActive.store(false);
    }

    /// CaptureSession 準備完了通知
    /// @param ctx 未使用
    /// @param session キャプチャセッション
    void onSessionReady(void * /*ctx*/, ACameraCaptureSession * /*session*/)
    {
        LOGI("NDK Camera session is ready");
    }

    /// AImageReader 新フレーム到着コールバック
    /// 最新フレームを取得して YUV プレーン情報を CameraSystem へ転送
    /// @param ctx 未使用
    /// @param reader AImageReader
    void onImageAvailable(void * /*ctx*/, AImageReader *reader)
    {
        if (!g_cameraActive.load())
            return;

        AImage *image = nullptr;
        media_status_t status = AImageReader_acquireLatestImage(reader, &image);
        if (status != AMEDIA_OK || !image)
            return;

        auto imageGuard = [](AImage *img)
        { if (img) AImage_delete(img); };
        std::unique_ptr<AImage, decltype(imageGuard)> imageCleanup(image, imageGuard);

        int32_t width = 0, height = 0;
        AImage_getWidth(image, &width);
        AImage_getHeight(image, &height);

        int32_t numPlanes = 0;
        AImage_getNumberOfPlanes(image, &numPlanes);
        if (numPlanes < 3)
            return;

        uint8_t *yData = nullptr;
        int yLen = 0;
        int yRow = 0;
        int yPix = 0;
        uint8_t *uData = nullptr;
        int uLen = 0;
        int uRow = 0;
        int uPix = 0;
        uint8_t *vData = nullptr;
        int vLen = 0;
        int vRow = 0;
        int vPix = 0;

        if (AImage_getPlaneData(image, 0, &yData, &yLen) != AMEDIA_OK ||
            AImage_getPlaneRowStride(image, 0, &yRow) != AMEDIA_OK ||
            AImage_getPlanePixelStride(image, 0, &yPix) != AMEDIA_OK ||
            AImage_getPlaneData(image, 1, &uData, &uLen) != AMEDIA_OK ||
            AImage_getPlaneRowStride(image, 1, &uRow) != AMEDIA_OK ||
            AImage_getPlanePixelStride(image, 1, &uPix) != AMEDIA_OK ||
            AImage_getPlaneData(image, 2, &vData, &vLen) != AMEDIA_OK ||
            AImage_getPlaneRowStride(image, 2, &vRow) != AMEDIA_OK ||
            AImage_getPlanePixelStride(image, 2, &vPix) != AMEDIA_OK)
        {
            return;
        }

        const int yWidth = width;
        const int yHeight = height;
        const int uvWidth = (width + 1) / 2;
        const int uvHeight = (height + 1) / 2;
        const int yRequired = yRow * (yHeight - 1) + yPix * (yWidth - 1) + 1;
        const int uRequired = uRow * (uvHeight - 1) + uPix * (uvWidth - 1) + 1;
        const int vRequired = vRow * (uvHeight - 1) + vPix * (uvWidth - 1) + 1;
        if (yLen < yRequired || uLen < uRequired || vLen < vRequired)
        {
            LOGE("Insufficient plane data length");
            return;
        }

        const int uvPixelStride = uPix; // expected 1 or 2
        CameraSystem::onYuvFrameRaw(yData, uData, vData, yRow, uRow, vRow, uvPixelStride, width, height);
    }

    /// カメラデバイス切断時コールバック
    /// アクティブ状態を false にし、ポインタを無効化
    /// @param ctx 未使用
    /// @param device カメラデバイス
    void onDeviceDisconnected(void * /*ctx*/, ACameraDevice *device)
    {
        LOGI("Camera device disconnected");
        if (device == g_camDevice)
        {
            g_cameraActive.store(false);
            g_camDevice = nullptr;
        }
    }

    /// カメラデバイスエラー発生時コールバック
    /// アクティブ状態を false にし、該当デバイスを無効化
    /// @param ctx 未使用
    /// @param device カメラデバイス
    /// @param error エラーコード
    void onDeviceError(void * /*ctx*/, ACameraDevice *device, int error)
    {
        LOGE("Camera device error: %d", error);
        if (device == g_camDevice)
        {
            g_cameraActive.store(false);
            g_camDevice = nullptr;
        }
    }

    /// NDK カメラ関連リソース一括クリーンアップ
    /// セッションの停止とクローズ、リクエスト/ターゲット/コンテナ/リーダ/デバイス/マネージャの順に
    /// 解放、状態を初期化
    void cleanupCameraResources()
    {
        LOGI("Cleaning up all camera resources");
        g_cameraActive.store(false);

        if (g_session)
        {
            ACameraCaptureSession_stopRepeating(g_session);
            ACameraCaptureSession_close(g_session);
            g_session = nullptr;
        }
        if (g_request)
        {
            ACaptureRequest_removeTarget(g_request, g_outputTarget);
            ACaptureRequest_free(g_request);
            g_request = nullptr;
        }
        if (g_outputTarget)
        {
            ACameraOutputTarget_free(g_outputTarget);
            g_outputTarget = nullptr;
        }
        if (g_sessionOutput && g_outputContainer)
        {
            ACaptureSessionOutputContainer_remove(g_outputContainer, g_sessionOutput);
            ACaptureSessionOutput_free(g_sessionOutput);
            g_sessionOutput = nullptr;
        }
        if (g_outputContainer)
        {
            ACaptureSessionOutputContainer_free(g_outputContainer);
            g_outputContainer = nullptr;
        }
        if (g_imageReader)
        {
            AImageReader_setImageListener(g_imageReader, nullptr);
            AImageReader_delete(g_imageReader);
            g_imageReader = nullptr;
        }
        if (g_camDevice)
        {
            ACameraDevice_close(g_camDevice);
            g_camDevice = nullptr;
        }
        if (g_camManager)
        {
            ACameraManager_delete(g_camManager);
            g_camManager = nullptr;
        }
        g_readerWindow = nullptr;
    }
}

/// NDK Camera2（camera2ndk + AImageReader）カメラプレビュー開始
/// バックカメラ優先でデバイスを選択し、YUV_420_888 + AImageReader でフレームを受領して CameraSystem に転送
/// @return 開始成功で JNI_TRUE
/// @note FPS 範囲はメタデータから高めのレンジを選択
extern "C" JNIEXPORT jboolean JNICALL
Java_com_kestrel_opensiv3d_MainActivity_startNdkCamera(JNIEnv *env, jobject /*thiz*/)
{
    if (g_cameraActive.load())
        return JNI_TRUE;

    cleanupCameraResources();

    g_camManager = ACameraManager_create();
    if (!g_camManager)
    {
        LOGE("Failed to create ACameraManager");
        return JNI_FALSE;
    }

    ACameraIdList *idList = nullptr;
    camera_status_t status = ACameraManager_getCameraIdList(g_camManager, &idList);
    if (status != ACAMERA_OK || !idList || idList->numCameras == 0)
    {
        LOGE("Failed to get camera ID list: %d", status);
        cleanupCameraResources();
        return JNI_FALSE;
    }

    auto idListGuard = [](ACameraIdList *p)
    { if (p) ACameraManager_deleteCameraIdList(p); };
    std::unique_ptr<ACameraIdList, decltype(idListGuard)> idListCleanup(idList, idListGuard);

    const char *chosenId = idList->cameraIds[0];
    for (int i = 0; i < idList->numCameras; ++i)
    {
        ACameraMetadata *meta = nullptr;
        camera_status_t metaStatus = ACameraManager_getCameraCharacteristics(g_camManager, idList->cameraIds[i], &meta);
        if (metaStatus == ACAMERA_OK && meta)
        {
            ACameraMetadata_const_entry entry{};
            if (ACameraMetadata_getConstEntry(meta, ACAMERA_LENS_FACING, &entry) == ACAMERA_OK && entry.count > 0)
            {
                if (entry.data.u8[0] == ACAMERA_LENS_FACING_BACK)
                {
                    chosenId = idList->cameraIds[i];
                    LOGI("Selected back camera: %s", chosenId);
                    ACameraMetadata_free(meta);
                    break;
                }
            }
            ACameraMetadata_free(meta);
        }
    }

    ACameraDevice_StateCallbacks *deviceCallbacks = new ACameraDevice_StateCallbacks{
        .context = nullptr,
        .onDisconnected = onDeviceDisconnected,
        .onError = onDeviceError};

    status = ACameraManager_openCamera(g_camManager, chosenId, deviceCallbacks, &g_camDevice);
    if (status != ACAMERA_OK || !g_camDevice)
    {
        LOGE("Failed to open camera: %d", status);
        cleanupCameraResources();
        return JNI_FALSE;
    }

    ACameraMetadata *meta = nullptr;
    status = ACameraManager_getCameraCharacteristics(g_camManager, chosenId, &meta);
    if (status != ACAMERA_OK || !meta)
    {
        LOGE("Failed to get camera characteristics: %d", status);
        cleanupCameraResources();
        return JNI_FALSE;
    }

    auto metaGuard = [](ACameraMetadata *p)
    {
        if (p)
            ACameraMetadata_free(p);
    };

    std::unique_ptr<ACameraMetadata, decltype(metaGuard)> metaCleanup(meta, metaGuard);

    ACameraMetadata_const_entry cfg{};
    std::vector<std::pair<int, int>> yuvOut;
    if (ACameraMetadata_getConstEntry(meta, ACAMERA_SCALER_AVAILABLE_STREAM_CONFIGURATIONS, &cfg) == ACAMERA_OK)
    {
        for (uint32_t i = 0; i + 3 < cfg.count; i += 4)
        {
            int32_t format = cfg.data.i32[i + 0];
            int32_t w = cfg.data.i32[i + 1];
            int32_t h = cfg.data.i32[i + 2];
            int32_t inFlag = cfg.data.i32[i + 3];
            if (inFlag == 0 && format == AIMAGE_FORMAT_YUV_420_888)
            {
                yuvOut.emplace_back(w, h);
                LOGI("YUV_420_888 supported size: %dx%d", w, h);
            }
        }
    }

    ACameraMetadata_const_entry fpsEntry{};
    int32_t minFps = 15000, maxFps = 30000; // デフォルト: 15-30fps
    if (ACameraMetadata_getConstEntry(meta, ACAMERA_CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES, &fpsEntry) == ACAMERA_OK)
    {
        if (fpsEntry.count >= 2)
        {
            size_t lastIdx = (fpsEntry.count / 2 - 1) * 2;
            minFps = fpsEntry.data.i32[lastIdx];
            maxFps = fpsEntry.data.i32[lastIdx + 1];
            LOGI("Using FPS range: %d-%d", minFps / 1000, maxFps / 1000);
        }
    }

    auto pickPref = [&](int w, int h)
    {
        return std::find(yuvOut.begin(), yuvOut.end(), std::pair{w, h}) != yuvOut.end();
    };

    int32_t width = 0, height = 0;
    if (pickPref(1280, 720))
    {
        width = 1280;
        height = 720;
    }
    else if (pickPref(960, 540))
    {
        width = 960;
        height = 540;
    }
    else if (pickPref(854, 480))
    {
        width = 854;
        height = 480;
    }
    else if (pickPref(640, 480))
    {
        width = 640;
        height = 480;
    }
    else if (!yuvOut.empty())
    {
        std::sort(yuvOut.begin(), yuvOut.end(), [](auto a, auto b)
                  { return 1LL * a.first * a.second < 1LL * b.first * b.second; });
        size_t mid = yuvOut.size() / 2;
        width = yuvOut[mid].first;
        height = yuvOut[mid].second;
        LOGI("Selected middle resolution: %dx%d", width, height);
    }
    else
    {
        LOGE("No supported YUV_420_888 resolutions found");
        cleanupCameraResources();
        return JNI_FALSE;
    }

    LOGI("Creating AImageReader with size %dx%d", width, height);

    media_status_t mediaStatus = AImageReader_new(width, height, AIMAGE_FORMAT_YUV_420_888, /*maxImages*/ 2, &g_imageReader);
    if (mediaStatus != AMEDIA_OK || !g_imageReader)
    {
        LOGE("Failed to create image reader: %d", mediaStatus);
        cleanupCameraResources();
        return JNI_FALSE;
    }

    mediaStatus = AImageReader_getWindow(g_imageReader, &g_readerWindow);
    if (mediaStatus != AMEDIA_OK || !g_readerWindow)
    {
        LOGE("Failed to get reader window: %d", mediaStatus);
        cleanupCameraResources();
        return JNI_FALSE;
    }

    AImageReader_ImageListener *imageListener = new AImageReader_ImageListener{
        .context = nullptr,
        .onImageAvailable = onImageAvailable};

    AImageReader_setImageListener(g_imageReader, imageListener);

    status = ACaptureSessionOutputContainer_create(&g_outputContainer);
    if (status != ACAMERA_OK)
    {
        LOGE("Failed to create output container: %d", status);
        cleanupCameraResources();
        return JNI_FALSE;
    }

    status = ACaptureSessionOutput_create(g_readerWindow, &g_sessionOutput);
    if (status != ACAMERA_OK)
    {
        LOGE("Failed to create session output: %d", status);
        cleanupCameraResources();
        return JNI_FALSE;
    }

    status = ACaptureSessionOutputContainer_add(g_outputContainer, g_sessionOutput);
    if (status != ACAMERA_OK)
    {
        LOGE("Failed to add output to container: %d", status);
        cleanupCameraResources();
        return JNI_FALSE;
    }

    status = ACameraOutputTarget_create(g_readerWindow, &g_outputTarget);
    if (status != ACAMERA_OK)
    {
        LOGE("Failed to create output target: %d", status);
        cleanupCameraResources();
        return JNI_FALSE;
    }

    status = ACameraDevice_createCaptureRequest(g_camDevice, TEMPLATE_PREVIEW, &g_request);
    if (status != ACAMERA_OK)
    {
        LOGE("Failed to create capture request: %d", status);
        cleanupCameraResources();
        return JNI_FALSE;
    }

    int32_t fpsRange[2] = {minFps, maxFps};
    ACaptureRequest_setEntry_i32(g_request, ACAMERA_CONTROL_AE_TARGET_FPS_RANGE, 2, fpsRange);

    status = ACaptureRequest_addTarget(g_request, g_outputTarget);
    if (status != ACAMERA_OK)
    {
        LOGE("Failed to add target to request: %d", status);
        cleanupCameraResources();
        return JNI_FALSE;
    }

    ACameraCaptureSession_stateCallbacks *sessionCallbacks = new ACameraCaptureSession_stateCallbacks{
        .context = nullptr,
        .onActive = onSessionActive,
        .onReady = onSessionReady,
        .onClosed = onSessionClosed};

    status = ACameraDevice_createCaptureSession(g_camDevice, g_outputContainer, sessionCallbacks, &g_session);
    if (status != ACAMERA_OK || !g_session)
    {
        LOGE("Failed to create capture session: %d", status);
        cleanupCameraResources();
        return JNI_FALSE;
    }

    status = ACameraCaptureSession_setRepeatingRequest(g_session, nullptr, 1, &g_request, nullptr);
    if (status != ACAMERA_OK)
    {
        LOGE("Failed to start repeating request: %d", status);
        cleanupCameraResources();
        return JNI_FALSE;
    }

    g_cameraActive.store(true);
    LOGI("Camera started successfully");
    return JNI_TRUE;
}

/// NDK カメラ停止、関連リソース解放
/// @return 常に JNI_TRUE
extern "C" JNIEXPORT jboolean JNICALL
Java_com_kestrel_opensiv3d_MainActivity_stopNdkCamera(JNIEnv *env, jobject /*thiz*/)
{
    LOGI("Stopping NDK camera");
    cleanupCameraResources();
    return JNI_TRUE;
}

// =====================================
// AGDK GameActivity Essential Functions
// =====================================

extern "C" JNIEXPORT jlong JNICALL
Java_com_google_androidgamesdk_GameActivity_initializeNativeCode(
		JNIEnv *env, jobject javaGameActivity, jstring internalDataDir,
		jstring obbDir,	jstring externalDataDir, jobject assetManager,
		jbyteArray savedState, jobject javaConfig)
{
	if (g_mainActivityInstance == nullptr)
	{
		g_mainActivityInstance = env->NewGlobalRef(javaGameActivity);
		LOGI("MainActivity global reference created for file dialog");
	}

    if (g_JavaVM == nullptr)
	{
		env->GetJavaVM(&g_JavaVM);
		LOGI("JavaVM saved for later use");
	}

    if (assetManager)
	{
		jobject globalAssetManager = env->NewGlobalRef(assetManager);
        s3d::g_AssetManager = AAssetManager_fromJava(env, globalAssetManager);
		LOGI("AssetManager initialized");
	}

    const char* internalDirStr = env->GetStringUTFChars(internalDataDir, nullptr);
	LOGI("Internal data dir: %s", internalDirStr);
	env->ReleaseStringUTFChars(internalDataDir, internalDirStr);
	return 1;
}

/// ネイティブ側の終了処理
/// @param env JNI 環境
extern "C" JNIEXPORT void JNICALL
Java_com_google_androidgamesdk_GameActivity_terminateNativeCode(
    JNIEnv *env, jobject /*obj*/, jlong /*handle*/)
{
	if (s3d::g_NativeWindow)
	{
		ANativeWindow_release(s3d::g_NativeWindow);
		s3d::g_NativeWindow = nullptr;
	}
	s3d::g_GameActivityHandle = nullptr;

	if (g_mainActivityInstance)
	{
		env->DeleteGlobalRef(g_mainActivityInstance);
		g_mainActivityInstance = nullptr;
	}
}

/// 直近の dlopen/dlsym 等のエラー文字列取得
/// 現実装は空文字列を返す。
/// @param env JNI 環境
/// @param obj GameActivity インスタンス
/// @return エラーメッセージ（UTF-8）
extern "C" JNIEXPORT jstring JNICALL 
Java_com_google_androidgamesdk_GameActivity_getDlError(
    JNIEnv *env, jobject /*obj*/)
{
    return env->NewStringUTF("");
}

/// Surface 生成時、ANativeWindow 取得
/// 既存の g_NativeWindow があれば解放して差し替える。
/// @param env JNI 環境
/// @param thiz MainActivity インスタンス
/// @param surface Java の Surface
/// @return 成功で JNI_TRUE、失敗で JNI_FALSE
extern "C" JNIEXPORT jboolean JNICALL
Java_com_google_androidgamesdk_GameActivity_onSurfaceCreatedNative(
    JNIEnv *env, jobject thiz, jlong /*handle*/, jobject surface)
{
    LOGI("onSurfaceCreatedNative - Recreating surface");
    if (g_NativeWindow != nullptr)
    {
        ANativeWindow_release(g_NativeWindow);
        g_NativeWindow = nullptr;
    }

    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
    if (window == nullptr)
    {
        LOGE("Failed to get native window from surface");
        return JNI_FALSE;
    }

    g_NativeWindow = window;
    s3d::g_NativeWindow = window;
    s3d::g_GameActivityHandle = window;

    if (!g_siv3d_thread_started)
    {
        LOGI("Initial surface created: %p", window);
        start_siv3d_thread();
    }
    else
    {
        LOGI("Resuming surface: %p", window);
        StartResume();
    }

    return JNI_TRUE;
}

/// Surface のサイズ/フォーマット変更時NativeWindow 更新
/// @param env JNI 環境
/// @param obj GameActivity インスタンス
/// @param surface Java の Surface
/// @param format ピクセルフォーマット
/// @param width 幅
/// @param height 高さ
extern "C" JNIEXPORT void JNICALL
Java_com_google_androidgamesdk_GameActivity_onSurfaceChangedNative(
    JNIEnv *env, jobject obj, jlong /*handle*/, jobject surface, jint format, jint width, jint height)
{
	LOGI("onSurfaceChangedNative: %dx%d format:%d", width, height, format);

	ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
	if (!window)
	{
		LOGE("onSurfaceChangedNative: Failed to get ANativeWindow from surface");
		return;
	}

	if (s3d::g_NativeWindow && s3d::g_NativeWindow != window)
	{
		LOGI("Releasing previous window: %p", s3d::g_NativeWindow);
		ANativeWindow_release(s3d::g_NativeWindow);
	}

	s3d::g_NativeWindow = window;
	s3d::g_GameActivityHandle = window;

	if (isSiv3DRunning())
	{
		try
		{
			bool result = SIV3D_ENGINE(Window)->resizeByFrameBufferSize(s3d::Size{width, height});
			if (!result)
			{
				LOGE("Failed to resize frame buffer: %dx%d", width, height);
			}
		}
		catch (const std::exception &e)
		{
			LOGE("Failed to notify Siv3D engine of size change: %s", e.what());
		}
	}
	else
	{
		LOGI("Siv3D engine not running, size change will be applied when engine starts");
	}
}

/// Surface 破棄時ANativeWindowを解放
/// @param env JNI 環境
/// @param obj GameActivity
extern "C" JNIEXPORT void JNICALL
Java_com_google_androidgamesdk_GameActivity_onSurfaceDestroyedNative(
    JNIEnv *env, jobject obj, jlong /*handle*/)
{
    if (s3d::g_NativeWindow)
    {
        ANativeWindow_release(s3d::g_NativeWindow);
        s3d::g_NativeWindow = nullptr;
        s3d::g_GameActivityHandle = nullptr;
    }
}

/// Game Text Inputの入力接続（ダミー実装）
extern "C" JNIEXPORT void JNICALL
Java_com_google_androidgamesdk_GameActivity_setInputConnectionNative(
    JNIEnv *env, jobject obj, jlong /*handle*/, jobject inputConnection)
{
    //LOGI("setInputConnectionNative - dummy implementation");
}

/// タッチイベント受信（AGDK互換ダミー実装）
/// @param env JNI 環境
/// @param obj GameActivity インスタンス
/// @param motionEvent android.view.MotionEvent
/// @return 取り扱った場合 JNI_TRUE、破棄時は JNI_FALSE
extern "C" JNIEXPORT jboolean JNICALL
Java_com_google_androidgamesdk_GameActivity_onTouchEventNative(
    JNIEnv *env, jobject obj, jlong /*handle*/, jobject motionEvent)
{
    // タッチ処理は MainActivity で処理
    return JNI_TRUE;
}

/// 物理キーボードのキー押下を処理し、Siv3D の Keyboard へ転送
/// @param env JNI 環境
/// @param obj GameActivity インスタンス
/// @param keyEvent android.view.KeyEvent
/// @return ハンドルした場合 JNI_TRUE
extern "C" JNIEXPORT jboolean JNICALL
Java_com_google_androidgamesdk_GameActivity_onKeyDownNative(
    JNIEnv *env, jobject obj, jlong /*handle*/, jobject keyEvent)
{
	jclass keyEventClass = env->FindClass("android/view/KeyEvent");
	jmethodID getKeyCodeMethod = env->GetMethodID(keyEventClass, "getKeyCode", "()I");
	jmethodID getRepeatCountMethod = env->GetMethodID(keyEventClass, "getRepeatCount", "()I");

	jint keyCode = env->CallIntMethod(keyEvent, getKeyCodeMethod);
	jint repeatCount = env->CallIntMethod(keyEvent, getRepeatCountMethod);

	LOG_TRACE(U"KeyDown: keyCode={}, repeatCount={}"_fmt(keyCode, repeatCount));

	auto keyboard = dynamic_cast<CKeyboard *>(SIV3D_ENGINE(Keyboard));
	if (keyboard)
	{
		keyboard->onKeyEvent(keyCode, true);
	}

	return JNI_TRUE;
}

/// 物理キーボードのキー解放を処理し、Siv3D の Keyboard へ転送
extern "C" JNIEXPORT jboolean JNICALL
Java_com_google_androidgamesdk_GameActivity_onKeyUpNative(
    JNIEnv *env, jobject obj, jlong /*handle*/, jobject keyEvent)
{
	jclass keyEventClass = env->FindClass("android/view/KeyEvent");
	jmethodID getKeyCodeMethod = env->GetMethodID(keyEventClass, "getKeyCode", "()I");
	jint keyCode = env->CallIntMethod(keyEvent, getKeyCodeMethod);

	LOG_TRACE(U"KeyUp: keyCode={}"_fmt(keyCode));

	auto keyboard = dynamic_cast<CKeyboard *>(SIV3D_ENGINE(Keyboard));
	if (keyboard)
	{
		keyboard->onKeyEvent(keyCode, false);
	}

	return JNI_TRUE;
}

/// Activity状態保存処理
/// @param env JNI 環境
/// @param obj GameActivity
/// @return 直列化した状態（jbyteArray）
extern "C" JNIEXPORT jbyteArray JNICALL 
Java_com_google_androidgamesdk_GameActivity_onSaveInstanceStateNative( 
    JNIEnv *env, jobject obj, jlong /*handle*/)
{
    SavedState state;
    memset(&state, 0, sizeof(SavedState));
    
    if (s3d::g_NativeWindow)
    {
        state.windowWidth = ANativeWindow_getWidth(s3d::g_NativeWindow);
        state.windowHeight = ANativeWindow_getHeight(s3d::g_NativeWindow);
    }
    else
    {
        LOGI("No window available for state saving");
    }
    
    state.engineInitialized = isSiv3DRunning();
    state.renderingSuspended = g_isRenderingSuspended;
    state.reinitAttempts = g_reinitAttempts;
    
    EGLContext currentContext = eglGetCurrentContext();
    if (currentContext != EGL_NO_CONTEXT && !state.renderingSuspended)
    {
        glGetIntegerv(GL_VIEWPORT, state.viewport);
        state.depthTestEnabled = glIsEnabled(GL_DEPTH_TEST) == GL_TRUE;
        state.blendEnabled = glIsEnabled(GL_BLEND) == GL_TRUE;
        
        GLenum glError = glGetError();
        if (glError != GL_NO_ERROR)
        {
            LOGE("OpenGL error during state saving: 0x%x", glError);
        }
    }
    
    jbyteArray result = env->NewByteArray(sizeof(SavedState));
    if (!result)
    {
        LOGE("Failed to create byte array for saved state");
        return env->NewByteArray(0);
    }
    
    env->SetByteArrayRegion(result, 0, sizeof(SavedState), reinterpret_cast<const jbyte *>(&state));
    
    if (env->ExceptionCheck())
    {
        LOGE("JNI exception occurred during state saving");
        env->ExceptionClear();
        return env->NewByteArray(0);
    }
    
    LOG_TRACE(U"Saved state: window={}x{}, engineInit={}, eglNeedsReinit={}, renderingSuspended={}"_fmt(
        state.windowWidth, state.windowHeight,
        state.engineInitialized ? U"true" : U"false",
        state.eglNeedsReinit ? U"true" : U"false",
        state.renderingSuspended ? U"true" : U"false"));
    
    return result;
}


/// 保存済みのActivity状態を取得して復元処理
/// @param env JNI 環境
/// @param obj GameActivity
/// @param savedState onSaveInstanceStateNative で保存したデータ
extern "C" JNIEXPORT void JNICALL
Java_com_google_androidgamesdk_GameActivity_onRestoreInstanceStateNative(
    JNIEnv *env, jobject obj, jlong /*handle*/, jbyteArray savedState)
{
    if (savedState != nullptr)
    {
        jsize length = env->GetArrayLength(savedState);
        if (length > 0)
        {
            jbyte *state = env->GetByteArrayElements(savedState, nullptr);
            env->ReleaseByteArrayElements(savedState, state, JNI_ABORT);
        }
    }
}

/// onStartコールバック
/// @param env JNI 環境
/// @param obj GameActivity インスタンス
extern "C" JNIEXPORT void JNICALL
Java_com_google_androidgamesdk_GameActivity_onStartNative(
    JNIEnv *env, jobject obj, jlong /*handle*/)
{
    LOGI("onStartNative - Starting application");
}

/// onStopネイティブコールバック
/// @param env JNI 環境
/// @param obj GameActivity インスタンス
extern "C" JNIEXPORT void JNICALL
Java_com_google_androidgamesdk_GameActivity_onStopNative(
    JNIEnv *env, jobject obj, jlong /*handle*/)
{
    LOGI("onStopNative - Stopping application");
    g_isRenderingSuspended = true;
    if (s3d::g_NativeWindow)
    {
        LOGI("Preserving surface while stopped");
    }
}

/// メモリ圧迫警告通知（AGDKで必要なダミー関数）
/// @param env JNI 環境
/// @param obj GameActivity インスタンス
/// @param level TRIM_MEMORY_* レベル
extern "C" JNIEXPORT void JNICALL
Java_com_google_androidgamesdk_GameActivity_onTrimMemoryNative(
    JNIEnv *env, jobject obj, jlong /*handle*/, jint level)
{
    LOGI("onTrimMemoryNative called: level %d", level);
}

/// サーフェス更新通知（AGDKで必要なダミー関数）
extern "C" JNIEXPORT void JNICALL
Java_com_google_androidgamesdk_GameActivity_onSurfaceRedrawNeededNative(
    JNIEnv *env, jobject obj, jlong /*handle*/, jobject surface)
{
    // MainActivityで描画処理
}

/// コンテント矩形変更通知（AGDKで必要なダミー関数）
extern "C" JNIEXPORT void JNICALL
Java_com_google_androidgamesdk_GameActivity_onContentRectChangedNative(
    JNIEnv *env, jobject obj, jlong handle, jint x, jint y, jint width, jint height)
{
    //LOGI("onContentRectChangedNative: x=%d, y=%d, w=%d, h=%d", x, y, width, height);
}

/// IME変更通知（AGDKで必要なダミー関数）
extern "C" JNIEXPORT void JNICALL
Java_com_google_androidgamesdk_GameActivity_onImeInsetsChangedNative(
    JNIEnv *env, jobject obj, jlong handle, jint x, jint y, jint width, jint height)
{
    //LOGI("onImeInsetsChangedNative: x=%d, y=%d, w=%d, h=%d", x, y, width, height);
}

/// Window変更通知（AGDKで必要なダミー関数）
extern "C" JNIEXPORT void JNICALL
Java_com_google_androidgamesdk_GameActivity_onWindowInsetsChangedNative(
    JNIEnv *env, jobject obj, jlong /*handle*/)
{
    //LOGI("onWindowInsetsChangedNative");
}

/// GameActivity の低レベル入力イベント（AGDKで必要なダミー関数）
extern "C" JNIEXPORT void JNICALL
Java_com_google_androidgamesdk_GameActivity_handleGameActivityInput(
    JNIEnv *env, jobject javaGameActivity, jlong /*handle*/, jobject event)
{
    LOGI("handleGameActivityInput");
}

/// ウィンドウのフォーカス状態変化通知（AGDKで必要なダミー関数）
extern "C" JNIEXPORT void JNICALL
Java_com_google_androidgamesdk_GameActivity_onWindowFocusChangedNative(
    JNIEnv *env, jobject obj, jlong /*handle*/, jboolean focused)
{
    LOGI("onWindowFocusChangedNative called: %s", focused ? "true" : "false");
}

/// 端末の構成変更通知（AGDKで必要なダミー関数）
extern "C" JNIEXPORT void JNICALL
Java_com_google_androidgamesdk_GameActivity_onConfigurationChangedNative(
    JNIEnv *env, jobject obj, jlong /*handle*/, jobject javaConfig)
{
    LOGI("onConfigurationChangedNative");
}
