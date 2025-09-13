package com.kestrel.opensiv3d

import android.app.Activity
import android.content.ContentUris
import android.content.Context
import android.content.Intent
import android.content.res.AssetManager
import android.hardware.camera2.*
import android.media.ImageReader
import android.net.Uri
import android.os.Bundle
import android.os.Environment
import android.os.Handler
import android.os.HandlerThread
import android.os.Looper
import android.util.Log
import android.view.*
import android.view.inputmethod.BaseInputConnection
import android.view.inputmethod.EditorInfo
import android.view.inputmethod.InputConnection
import android.view.inputmethod.InputMethodManager
import android.view.WindowManager
import android.util.DisplayMetrics 
import android.util.Size
import android.Manifest
import android.content.pm.PackageManager
import androidx.core.content.ContextCompat
import androidx.core.app.ActivityCompat
import android.widget.Toast
import android.graphics.ImageFormat
import android.media.Image
import android.provider.DocumentsContract
import android.provider.MediaStore

import java.io.File
import java.io.FileOutputStream
import java.io.InputStream
import java.nio.ByteBuffer
import java.util.concurrent.Semaphore
import java.util.concurrent.TimeUnit

import com.google.androidgamesdk.GameActivity

/// @class MainActivity
/// @brief アプリケーションのメインアクティビティ
///
/// MainActivityを継承し、Siv3Dネイティブエンジンとの連携、
/// 入力イベントの処理、IMEの制御、カメラ機能などを行います
class MainActivity : GameActivity() {
    companion object {
        private const val TAG = "MainActivity"
        private const val USE_NDK_CAMERA = true
        
        /// @brief タッチダウンのアクションを示す定数
        private const val ACTION_DOWN = 0
        /// @brief タッチアップのアクションを示す定数
        private const val ACTION_UP = 1
        /// @brief タッチムーブのアクションを示す定数
        private const val ACTION_MOVE = 2
        /// @brief ポインターダウンのアクションを示す定数（マルチタッチ用）
        private const val ACTION_POINTER_DOWN = 5
        /// @brief ポインターアップのアクションを示す定数（マルチタッチ用）
        private const val ACTION_POINTER_UP = 6
        
        // キーコード定数
        /// @brief 全角/半角キーのキーコード
        private const val KEY_GRAVE = 68 // 全角/半角キー
        
        // ファイル選択関連の定数
        /// @brief ファイル選択リクエストのコード
        private const val FILE_SELECT_CODE = 1001

        // カメラ関連の定数
        /// @brief カメラ権限リクエストのコード
        private const val CAMERA_PERMISSION_REQUEST_CODE = 100
        /// @brief プレビューの最大幅
        private const val MAX_PREVIEW_WIDTH = 1280
        /// @brief プレビューの最大高さ
        private const val MAX_PREVIEW_HEIGHT = 720
        /// @brief 最大カメラエラー回数
        private const val MAX_CAMERA_ERRORS = 3
        init {
            System.loadLibrary("opensiv3d")
        }
    }

    /// @brief 初回のフレームバッファサイズを送信済みかどうかのフラグ
    private var hasSentInitialFrameBufferSize = false
    
    /// @brief Siv3Dエンジンが初期化されたかどうかを追跡するフラグ
    private var isSiv3DEngineInitialized = false
    
    // 初期化されるまで溜めておくサイズ値
    /// @brief エンジン初期化前に保留されている画面の幅
    private var pendingWidth = 0
    /// @brief エンジン初期化前に保留されている画面の高さ
    private var pendingHeight = 0
    /// @brief エンジン初期化前に保留されている画面の密度
    private var pendingDensity = 0f
    
    // IME関連の変数
    /// @brief InputMethodManagerのインスタンス
    private var imeManager: InputMethodManager? = null
    /// @brief IMEが表示されているかどうかのフラグ
    private var isImeVisible = false
    /// @brief 全角入力モードかどうかのフラグ
    private var isFullWidthMode = false  // デフォルトは半角モード
    
    // ファイル選択関連の変数
    /// @brief ファイル選択の結果パス（null = 未選択、空文字 = キャンセル、パス = 選択済み）
    private var fileSelectionResult: String? = null
    /// @brief ファイル選択が進行中かどうかのフラグ
    private var isFileSelectionInProgress = false

    // カメラ用バックグラウンドスレッド
    private var backgroundThread: HandlerThread? = null
    private var backgroundHandler: Handler? = null
    /// @brief ネイティブ層の初期化処理を呼び出します
    /// @param assetManager アセットマネージャー
    /// @param internalFilesDir 内部ストレージのファイルディレクトリパス
    /// @param externalFilesDir 外部ストレージのファイルディレクトリパス
    /// @param cacheDir キャッシュディレクトリパス
    /// @return 初期化が成功したかどうか
    private external fun onCreateNative(assetManager: AssetManager, internalDataPath: String, externalDataPath: String, packageName: String ): Boolean
    
    
    /// @brief キーダウンイベントをネイティブ層に通知します
    /// @param keyCode 押されたキーのコード
    /// @return イベントが処理されたかどうか
    private external fun onKeyDownNative(keyCode: Int): Boolean
    /// @brief キーアップイベントをネイティブ層に通知します
    /// @param keyCode 離されたキーのコード
    /// @return イベントが処理されたかどうか
    private external fun onKeyUpNative(keyCode: Int): Boolean
    /// @brief テキスト入力イベントをネイティブ層に通知します
    /// @param text 入力されたテキスト
    /// @return イベントが処理されたかどうか
    private external fun onTextInputNative(text: String): Boolean // テキスト入力用

    /// @brief Androidフレームワークのフレームバッファサイズをネイティブ層に通知します
    /// @param width 幅
    /// @param height 高さ
    private external fun SendFrameBufferSizeNative(width: Int, height: Int)

    /// @brief CWindowのフレームバッファサイズ変更をネイティブ層に通知します
    /// @param width 新しい幅
    /// @param height 新しい高さ
    private external fun onFrameBufferSizeNative(width: Int, height: Int)

    /// @brief スケーリング（画面密度）の変更をネイティブ層に通知します
    /// @param sx X方向のスケーリングファクター
    /// @param sy Y方向のスケーリングファクター
    private external fun onScalingChangeNative(sx: Float, sy: Float)

    /// @brief ネイティブ層にGameActivityのインスタンスをキャッシュさせます
    private external fun SetMainActivityNative()

    /// @brief タッチイベントをネイティブ層に通知します
    /// @param action タッチアクション（DOWN, UP, MOVE）
    /// @param x X座標
    /// @param y Y座標
    private external fun onTouchEventNative(action: Int, x: Int, y: Int)

    /// @brief マルチタッチイベントをネイティブ層に通知します
    /// @param action タッチアクション（DOWN, UP, MOVE）
    /// @param pointerId ポインターID
    /// @param x X座標
    /// @param y Y座標
    private external fun onMultiTouchEventNative(action: Int, pointerId: Int, x: Int, y: Int)
    
    /// @brief マウスイベントとしてネイティブ層に通知します
    /// @details タッチイベントをマウスイベントとしても解釈し、カーソル位置の更新などに利用します
    /// @param action マウスアクション（DOWN, UP, MOVE）
    /// @param x X座標
    /// @param y Y座標
    private external fun onMouseEventNative(action: Int, x: Int, y: Int)

    /// @brief IMEのフォーカス状態をネイティブ層に通知します
    /// @param focused フォーカス状態（true: フォーカス取得, false: フォーカス失去）
    /// @return 処理が成功したかどうか
    private external fun onTextInputFocusNative(focused: Boolean): Boolean
    
    /// @brief 現在の入力テキストをネイティブ層から取得します
    /// @return 現在入力中のテキスト
    private external fun GetCurrentTextNative(): String

    /// @brief マウス移動イベントをネイティブ層に通知します
    /// @param x X座標
    /// @param y Y座標
    private external fun onMoveNative(x: Int, y: Int)

    /// @brief ウィンドウリサイズイベントをネイティブ層に通知します
    /// @param width 新しい幅
    /// @param height 新しい高さ
    private external fun onResizeNative(width: Int, height: Int)

    /// @brief アクティビティが一時停止したことをネイティブ層に通知します
    /// @return 処理が成功したかどうか
    private external fun onPauseNative(): Boolean
    
    /// @brief ファイル選択結果をネイティブ層に通知します
    /// @param filePath 選択されたファイルのパス（空文字の場合はキャンセル）
    private external fun onFileSelectedNative(filePath: String)
    
    /// @brief ネイティブ層から呼び出されるファイル選択開始メソッド
    /// @details JNIからこのメソッドが呼ばれ、Androidのファイル選択ダイアログを表示します
    external fun startFileSelection(): Boolean
    
    /// @brief MainActivityのグローバル参照をファイル選択用にネイティブ層に設定します
    private external fun setMainActivityForFileSelection()
    
    /// @brief アクティビティが再開されたことをネイティブ層に通知します
    /// @return 処理が成功したかどうか
    private external fun onResumeNative(): Boolean

    /// @brief カーソル更新イベントをネイティブ層に通知します
    /// @param x X座標
    /// @param y Y座標
    private external fun onCursorUpdateNative(x: Int, y: Int)
    
    /// @brief BTマウスボタン状態をネイティブ層に通知します
    /// @param buttonState ボタン状態
    private external fun onMouseButtonNative(buttonState: Int)


    /// @brief アクティビティが最初に作成されるときに呼び出されます
    /// @details ビューの初期化、ネイティブコードの初期化、各種マネージャーの設定を行います
    /// @param savedInstanceState 以前の状態が保存されている場合、その情報を含むBundle
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        Log.d(TAG, "onCreate")
        
        // キーリスナーを最初に設定（優先的に処理するため）
        setupKeyDetector()
        
        // IMEマネージャーの初期化
        imeManager = getSystemService(Context.INPUT_METHOD_SERVICE) as InputMethodManager
        
        // IMEモードの初期設定（WindowManagerの設定）
        window.addFlags(WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM)
        
        // アセットマネージャーとファイルパスをネイティブに渡す
        val assetManager = assets
        val internalDataPath = filesDir.absolutePath
        val externalDataPath = getExternalFilesDir(null)?.absolutePath ?: ""
        val packageName = packageName
        
        onCreateNative(assetManager, internalDataPath, externalDataPath, packageName)
        
		// ファイル選択用のMainActivityグローバル参照を設定
        setMainActivityForFileSelection()
        
        // カスタムInputConnectionプロバイダビューを作成
        val rootView = findViewById<ViewGroup>(android.R.id.content)
        val inputView = Siv3DInputView(this)
        rootView.addView(inputView)
        
        // 最後にシステムUIを非表示
        hideSystemUi()

        // 画面サイズ取得（ナビゲーションバーを含む実際の画面サイズ）
        val displayMetrics = DisplayMetrics()
        windowManager.defaultDisplay.getRealMetrics(displayMetrics)
        val realWidth = displayMetrics.widthPixels
        val realHeight = displayMetrics.heightPixels

        Log.d(TAG, "Real screen size: $realWidth x $realHeight, density: ${displayMetrics.density}")
        
        // Send initial frame buffer size immediately (AGDK-compatible)
        SendFrameBufferSizeNative(realWidth, realHeight)
        hasSentInitialFrameBufferSize = true
    }

    /// @brief アクティビティが再開されるときに呼び出されます
    /// @details バックグラウンドスレッドの開始とカメラの初期化を行います
    override fun onResume() {
        super.onResume()
        Log.d(TAG, "onResume")
    }

    /// @brief アクティビティが一時停止されるときに呼び出されます
    /// @details カメラの終了とバックグラウンドスレッドの停止を行います
    override fun onPause() {
        super.onPause()
        Log.d(TAG, "onPause")
    }



    /// @brief 全角/半角キー（Graveキー）の入力を検出するためのリスナーを設定します
    /// @details IMEとの兼ね合いで通常のonKeyDownでは補足しきれない場合があるため、
    ///          複数のリスナーでキー入力を監視します
    private fun setupKeyDetector() {
        // アクティビティ全体にキーリスナーを設定
        val contentView = findViewById<ViewGroup>(android.R.id.content)
        contentView.isFocusable = true
        contentView.isFocusableInTouchMode = true
        contentView.requestFocus()
        
        // バックアップキーリスナー
        contentView.setOnKeyListener { _, keyCode, event ->
            Log.d(TAG, "ContentView.onKey: keyCode=$keyCode, action=${event.action}")
            
            // 全角/半角キーのみ特別処理
            if (keyCode == KEY_GRAVE) {
                // UPイベントでモード切替を実行
                if (event.action == KeyEvent.ACTION_UP) {
                    Handler(Looper.getMainLooper()).post {
                        toggleInputMode()
                    }
                    return@setOnKeyListener true
                }
            }
            false
        }
    }

    /// @brief IMEの入力モード（全角/半角）を切り替えます
    /// @details isFullWidthModeフラグを反転させ、IMEの表示/非表示と設定を更新します
    private fun toggleInputMode() {
        isFullWidthMode = !isFullWidthMode
        Log.d(TAG, "TOGGLE MODE: " + if (isFullWidthMode) "JAPANESE INPUT" else "ENGLISH INPUT")
        
        try {
            if (isFullWidthMode) {
                // 日本語モード - IMEを有効化
                window.clearFlags(WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM)
                
                val view = findViewById<Siv3DInputView>(R.id.siv3d_input_view)
                if (view != null) {
                    view.requestFocus()
                    isImeVisible = true
                    
                    // Notify native layer (AGDK-compatible function)
                    onTextInputFocusNative(true)
                    
                    imeManager?.showSoftInput(view, InputMethodManager.SHOW_FORCED)
                    
                    // 入力メソッドを更新
                    imeManager?.restartInput(view)
                }
            } else {
                // 英数モード - IMEを非表示化
                val view = findViewById<Siv3DInputView>(R.id.siv3d_input_view)
                if (view != null) {
                    // IMEを非表示
                    imeManager?.hideSoftInputFromWindow(view.windowToken, 0)
                    isImeVisible = false
                    
                    // Notify native layer (AGDK-compatible function)
                    onTextInputFocusNative(false)
                }
                
                // IMEを完全に閉じる
                window.addFlags(WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM)
            }
        } catch (e: Exception) {
            Log.e(TAG, "Error toggling input mode", e)
        }
    }

    /// @brief ハードウェアキーが押されたときに呼び出されます
    /// @details GraveキーによるIMEモード切替や、BS, DEL, Enterなどの特殊キーを処理し、
    ///          ネイティブ層にイベントを転送します
    /// @param keyCode 押されたキーのコード
    /// @param event キーイベントの詳細情報
    /// @return イベントを消費した場合はtrue、そうでない場合はfalse
    override fun onKeyDown(keyCode: Int, event: KeyEvent): Boolean {
        
        // 68（全角/半角キー）を直接検出
        if (keyCode == KEY_GRAVE) {
            toggleInputMode()
            return true
        }
        
        // 特殊キーの処理
        when (keyCode) {
            KeyEvent.KEYCODE_DEL -> {
                onTextInputNative("\b")  // バックスペースを送信
            }
            KeyEvent.KEYCODE_FORWARD_DEL -> {
                onTextInputNative("\u007F")  // DELを送信
            }
            KeyEvent.KEYCODE_ENTER -> {
                val keyHandled = onKeyDownNative(keyCode)
                onTextInputNative("\u000D")
                return keyHandled
            }
            KeyEvent.KEYCODE_TAB -> {
                onTextInputNative("\t")  // タブを送信
            }
            else -> {
                // 半角モードのみ文字入力を処理
                if (!isFullWidthMode) {
                    // 通常キーの処理
                    val character = event.unicodeChar.toChar()
                    
                    // 有効な文字の場合のみテキスト入力として処理
                    if (character.code > 0 && (character.isLetterOrDigit() || character.isWhitespace() || character in ".,;:!?@#$%^&*()_+-=[]{}\\|'\"/")) {
                        onTextInputNative(character.toString())
                    }
                }
            }
        }
        
        return onKeyDownNative(keyCode)
    }

    override fun onKeyUp(keyCode: Int, event: KeyEvent): Boolean {
        return onKeyUpNative(keyCode)  // AGDK-compatible function
    }

    /// @brief キーイベントがウィンドウにディスパッチされる際に呼び出されます
    /// @details 他のリスナーよりも先にGraveキーを補足するためのバックアップ処理です
    /// @param event ディスパッチされるキーイベント
    /// @return イベントを消費した場合はtrue、親クラスの処理に任せる場合はsuperの戻り値
    override fun dispatchKeyEvent(event: KeyEvent): Boolean {
        val keyCode = event.keyCode
        
        // 全角/半角キーを直接検出
        if (keyCode == KEY_GRAVE) {
            
            // UPイベントでモード切替を実行
            if (event.action == KeyEvent.ACTION_UP) {
                toggleInputMode()
            }
            return true
        }
        
        // その他のキーは通常処理
        return super.dispatchKeyEvent(event)
    }

    /// @class Siv3DInputView
    /// @brief IMEからのテキスト入力を受け取るためのカスタムビュー
    /// @details フォーカスを受け取り、InputConnectionを生成してIMEとSiv3Dエンジンを仲介します
    ///          サイズが1x1の見えないビューとしてレイアウトに配置されます
    /// @param context このビューが実行されているコンテキスト
    inner class Siv3DInputView(context: Context) : View(context) {
        init {
            id = R.id.siv3d_input_view
            isFocusable = true
            isFocusableInTouchMode = true
            
            // レイアウトパラメータ設定
            val params = ViewGroup.LayoutParams(1, 1) // 最小サイズ
            layoutParams = params
        }
        
        /// @brief このビューがフォーカスを持っているときのキーダウンイベントを処理します
        /// @details IMEが処理しない制御キーなどをGameActivityのハンドラに転送します
        /// @param keyCode 押されたキーのコード
        /// @param event キーイベント
        /// @return イベントを処理した場合はtrue
        override fun onKeyDown(keyCode: Int, event: KeyEvent): Boolean {
            Log.d(TAG, "InputView.onKeyDown: keyCode=$keyCode")
            
            // 全角/半角キーと制御キーは必ず転送
            if (keyCode == KeyEvent.KEYCODE_GRAVE || isControlKey(keyCode)) {
                // アクティビティのキーハンドラに転送
                return this@MainActivity.onKeyDown(keyCode, event)
            }
            
            // その他のキーは通常処理
            return super.onKeyDown(keyCode, event)
        }
        
        /// @brief このビューがフォーカスを持っているときのキーアップイベントを処理します
        /// @details IMEが処理しない制御キーなどをGameActivityのハンドラに転送します
        /// @param keyCode 離されたキーのコード
        /// @param event キーイベント
        /// @return イベントを処理した場合はtrue
        override fun onKeyUp(keyCode: Int, event: KeyEvent): Boolean {
            // 全角/半角キーと制御キーは必ず転送
            if (keyCode == KeyEvent.KEYCODE_GRAVE || isControlKey(keyCode)) {
                // アクティビティのキーハンドラに転送
                return this@MainActivity.onKeyUp(keyCode, event)
            }
            
            // その他のキーは通常処理
            return super.onKeyUp(keyCode, event)
        }
        
        /// @brief IMEとの通信チャネルであるInputConnectionを生成します
        /// @param outAttrs IMEの属性を定義するためのEditorInfoオブジェクト
        /// @return 全角モードの場合にカスタムInputConnectionを、それ以外はnullを返します
        override fun onCreateInputConnection(outAttrs: EditorInfo): InputConnection? {
            Log.d(TAG, "onCreateInputConnection - fullWidthMode: $isFullWidthMode")
            
            // 半角モードでは接続を提供しない（nullを返す）
            if (!isFullWidthMode && !isImeVisible) {
                Log.d(TAG, "Returning null InputConnection - half-width mode active")
                return null
            }
            
            // 日本語IMEを使う設定
            outAttrs.inputType = EditorInfo.TYPE_CLASS_TEXT
            
            if (isFullWidthMode) {
                // 全角モード（日本語入力）の設定
                outAttrs.inputType = outAttrs.inputType or EditorInfo.TYPE_TEXT_VARIATION_NORMAL
                // 日本語入力を促進するためのヒント
                outAttrs.imeOptions = EditorInfo.IME_ACTION_NONE or EditorInfo.IME_FLAG_NO_EXTRACT_UI
            } else {
                // 半角モード（英数入力）の設定
                outAttrs.inputType = outAttrs.inputType or EditorInfo.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD
                outAttrs.imeOptions = EditorInfo.IME_ACTION_NONE or EditorInfo.IME_FLAG_NO_EXTRACT_UI or EditorInfo.IME_FLAG_NO_FULLSCREEN
            }
            
            // カスタムInputConnectionを使って特殊キーの入力を捕捉
            return object : BaseInputConnection(this, true) {

                /// @brief IMEからキーイベントが送信されたときに呼び出されます
                /// @param event IMEからのキーイベント
                /// @return イベントを処理した場合はtrue
                override fun sendKeyEvent(event: KeyEvent): Boolean {
                    val keyCode = event.keyCode
                    Log.d(TAG, "InputConnection.sendKeyEvent: keyCode=$keyCode, action=${event.action}")

                    // 全角/半角キーと制御キーは常にアクティビティに転送
                    if (keyCode == KeyEvent.KEYCODE_GRAVE || isControlKey(keyCode)) {
                        if (event.action == KeyEvent.ACTION_DOWN) {
                            return this@MainActivity.onKeyDown(keyCode, event)
                        } else if (event.action == KeyEvent.ACTION_UP) {
                            return this@MainActivity.onKeyUp(keyCode, event)
                        }
                    }
                    
                    // その他のキーはデフォルト処理
                    return super.sendKeyEvent(event)
                }
                
                /// @brief IMEがテキストを確定したときに呼び出されます
                /// @param text 確定されたテキスト
                /// @param newCursorPosition 新しいカーソル位置
                /// @return 常にtrueを返します
                override fun commitText(text: CharSequence?, newCursorPosition: Int): Boolean {
                    Log.d(TAG, "commitText: $text")
                    if (text != null && text.isNotEmpty()) {
                        // テキストをSiv3Dエンジンに送信
                        onTextInputNative(text.toString())
                    }
                    return true
                }
                
                /// @brief IMEが周囲のテキストの削除を要求したときに呼び出されます
                /// @details 主にバックスペースキーの処理に使われます
                /// @param beforeLength カーソル前の削除文字数
                /// @param afterLength カーソル後の削除文字数
                /// @return 常にtrueを返します
                override fun deleteSurroundingText(beforeLength: Int, afterLength: Int): Boolean {
                    Log.d(TAG, "deleteSurroundingText: before=$beforeLength, after=$afterLength")
                    // バックスペースをシミュレート
                    if (beforeLength > 0) {
                        for (i in 0 until beforeLength) {
                            onTextInputNative("\b")
                        }
                    }
                    return true
                }
            }
        }
    }

    /// @brief アプリケーション内で使用するリソースIDを定義します
    object R {
        object id {
            /// @brief Siv3DInputViewに割り当てるビューID
            const val siv3d_input_view = 1001
        }
    }

    /// @brief 指定されたキーコードが制御キーかどうかを判定します
    /// @details 修飾キーやファンクションキーなど、IMEで直接文字入力に関わらないキーを指します
    /// @param keyCode 判定するキーコード
    /// @return 制御キーの場合はtrue
    private fun isControlKey(keyCode: Int): Boolean {
        return keyCode == KeyEvent.KEYCODE_GRAVE ||    // 全角/半角
               keyCode == KeyEvent.KEYCODE_SHIFT_LEFT ||
               keyCode == KeyEvent.KEYCODE_SHIFT_RIGHT ||
               keyCode == KeyEvent.KEYCODE_CTRL_LEFT ||
               keyCode == KeyEvent.KEYCODE_CTRL_RIGHT ||
               keyCode == KeyEvent.KEYCODE_ALT_LEFT ||
               keyCode == KeyEvent.KEYCODE_ALT_RIGHT ||
               keyCode == KeyEvent.KEYCODE_ESCAPE ||
               keyCode in KeyEvent.KEYCODE_F1..KeyEvent.KEYCODE_F12
    }



    /// @brief ネイティブのSiv3Dエンジンが初期化されたときにJNI経由で呼び出されます
    /// @details エンジンが利用可能になったことを示し、保留していた処理を実行します
    fun onSiv3DEngineInitialized() {
        Log.d(TAG, "Siv3D Engine initialized!")
        isSiv3DEngineInitialized = true
        
        // 保留中のサイズを送信
        if (pendingWidth > 0 && pendingHeight > 0) {
            sendFrameBufferSizeToNative()
        }
    }

    /// @brief 画面へのタッチイベントを処理します
    /// @details タッチアクションをネイティブのアクションに変換し、座標とともに転送します
    /// @param event タッチイベントオブジェクト
    /// @return イベントを処理した場合はtrue
    override fun onTouchEvent(event: MotionEvent): Boolean {
        val actionMasked = event.actionMasked
        val action = when(actionMasked) {
            MotionEvent.ACTION_DOWN -> ACTION_DOWN
            MotionEvent.ACTION_UP, MotionEvent.ACTION_CANCEL -> ACTION_UP
            MotionEvent.ACTION_MOVE -> ACTION_MOVE
            MotionEvent.ACTION_POINTER_DOWN -> ACTION_POINTER_DOWN
            MotionEvent.ACTION_POINTER_UP -> ACTION_POINTER_UP
            else -> return false
        }
        
        // MOVE イベントの場合は、全てのアクティブなポインターを処理
        if (actionMasked == MotionEvent.ACTION_MOVE) {
            // すべてのポインターを処理
            for (i in 0 until event.pointerCount) {
                val originalId = event.getPointerId(i)
                val offsetId = originalId + 1  // 0をBTマウス用に予約
                val x = event.getX(i)
                val y = event.getY(i)
                onMultiTouchEventNative(action, offsetId, x.toInt(), y.toInt())
            }
        } else {
            // DOWN/UP イベントの場合は、対象のポインターのみ処理
            val pointerIndex = when(actionMasked) {
                MotionEvent.ACTION_POINTER_DOWN, MotionEvent.ACTION_POINTER_UP -> {
                    (event.action and MotionEvent.ACTION_POINTER_INDEX_MASK) shr 
                        MotionEvent.ACTION_POINTER_INDEX_SHIFT
                }
                else -> 0 // ACTION_DOWN, ACTION_UPの場合は常に0
            }
            
            val originalId = event.getPointerId(pointerIndex)
            val offsetId = originalId + 1  // 0をBTマウス用に予約
            val x = event.getX(pointerIndex)
            val y = event.getY(pointerIndex)
            
            onMultiTouchEventNative(action, offsetId, x.toInt(), y.toInt())
        }
        
        return true
    }


    /// @brief システムUI（ナビゲーションバー、ステータスバー）を非表示にします
    /// @details イマーシブモードを有効にし、全画面表示を実現します
    private fun hideSystemUi() {
        val decorView = window.decorView
        decorView.systemUiVisibility = (View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                or View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                or View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                or View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                or View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                or View.SYSTEM_UI_FLAG_FULLSCREEN)
    }

    /// @brief ネイティブ層からフレームバッファサイズの変更通知を受け取ります (JNI経由)
    /// @param width 新しい幅
    /// @param height 新しい高さ
    fun onFrameBufferSizeChanged(width: Int, height: Int) {
        Log.d(TAG, "onFrameBufferSizeChanged: $width x $height")
        
        pendingWidth = width
        pendingHeight = height
        
        // CWindowにフレームバッファサイズの変更を通知（エンジン初期化後のみ）
        if (isSiv3DEngineInitialized) {
            onFrameBufferSizeNative(width, height)
        }
    }

    /// @brief 現在の画面サイズと密度をネイティブ層に送信します
    /// @details Androidフレームワーク用とSiv3Dエンジン(CWindow)用の両方の関数を呼び出します
    private fun sendFrameBufferSizeToNative() {
        val displayMetrics = DisplayMetrics()
        windowManager.defaultDisplay.getRealMetrics(displayMetrics)
        val width = displayMetrics.widthPixels
        val height = displayMetrics.heightPixels
        val density = displayMetrics.density
        
        // 最新の値を保存
        pendingWidth = width
        pendingHeight = height
        pendingDensity = density
        
        // Androidフレームワーク用の通知（常に呼び出す）
        SendFrameBufferSizeNative(width, height)
        
        // CWindow用の通知（エンジン初期化後のみ）
        if (isSiv3DEngineInitialized) {
            onFrameBufferSizeNative(width, height)
            onScalingChangeNative(density, density)
        }
        
        hasSentInitialFrameBufferSize = true
        
        Log.d(TAG, "Sent frame buffer size to native: $width x $height")
    }
    /// @brief ウィンドウのフォーカス状態が変更されたときに呼び出されます
    override fun onWindowFocusChanged(hasFocus: Boolean) {
        Log.d(TAG, "onWindowFocusChanged: $hasFocus")
        super.onWindowFocusChanged(hasFocus)
        if (hasFocus) {
            hideSystemUi()
        }
    }

    /// @brief ファイル選択ダイアログを開始します
    /// @details ネイティブ層から呼び出され、Androidの標準ファイル選択インテントを表示します
    /// @return ファイル選択が開始できたかどうか
    fun startFileSelectionDialog(): Boolean {
        return try {
            if (isFileSelectionInProgress) {
                Log.w(TAG, "File selection already in progress")
                return false
            }

            // ファイル選択インテントを作成
            val intent = Intent(Intent.ACTION_GET_CONTENT).apply {
                type = "image/*"  // 画像ファイルのみ
                addCategory(Intent.CATEGORY_OPENABLE)
                putExtra(Intent.EXTRA_LOCAL_ONLY, true)
            }

            // ファイル選択を開始
            isFileSelectionInProgress = true
            fileSelectionResult = null
            
            startActivityForResult(
                Intent.createChooser(intent, "画像を選択"),
                FILE_SELECT_CODE
            )
            
            Log.d(TAG, "File selection dialog started")
            true
        } catch (e: Exception) {
            Log.e(TAG, "Failed to start file selection dialog", e)
            isFileSelectionInProgress = false
            false
        }
    }

    /// @brief アクティビティの結果を処理します
    /// @param requestCode リクエストコード
    /// @param resultCode 結果コード
    /// @param data 結果データ
    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        
        if (requestCode == FILE_SELECT_CODE) {
            isFileSelectionInProgress = false
            
            if (resultCode == Activity.RESULT_OK && data?.data != null) {
                val uri = data.data!!
                val filePath = getRealPathFromURI(uri)
                
                if (filePath != null) {
                    Log.d(TAG, "File selected: $filePath")
                    fileSelectionResult = filePath
                    onFileSelectedNative(filePath)
                } else {
                    Log.w(TAG, "Could not get real path from URI: $uri")
                    fileSelectionResult = ""
                    onFileSelectedNative("")
                }
            } else {
                Log.d(TAG, "File selection cancelled")
                fileSelectionResult = ""
                onFileSelectedNative("")
            }
        }
    }

    /// @brief URIから実際のファイルパスを取得します
    /// @param uri ファイルのURI
    /// @return 実際のファイルパス（取得できない場合はnull）
    private fun getRealPathFromURI(uri: Uri): String? {
        return try {
            when {
                // MediaStore URI
                uri.toString().startsWith("content://media/") -> {
                    getPathFromMediaStore(uri)
                }
                // Document URI
                DocumentsContract.isDocumentUri(this, uri) -> {
                    getPathFromDocument(uri)
                }
                // Content URI
                uri.scheme == "content" -> {
                    copyUriToTempFile(uri)
                }
                // File URI
                uri.scheme == "file" -> {
                    uri.path
                }
                else -> {
                    copyUriToTempFile(uri)
                }
            }
        } catch (e: Exception) {
            Log.e(TAG, "Error getting real path from URI", e)
            null
        }
    }

    /// @brief MediaStoreからファイルパスを取得します
    private fun getPathFromMediaStore(uri: Uri): String? {
        val projection = arrayOf(MediaStore.Images.Media.DATA)
        contentResolver.query(uri, projection, null, null, null)?.use { cursor ->
            val columnIndex = cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA)
            cursor.moveToFirst()
            return cursor.getString(columnIndex)
        }
        return null
    }

    /// @brief DocumentProviderからファイルパスを取得します
    private fun getPathFromDocument(uri: Uri): String? {
        val docId = DocumentsContract.getDocumentId(uri)
        
        return when {
            // External Storage Provider
            uri.authority == "com.android.externalstorage.documents" -> {
                val split = docId.split(":")
                if (split[0] == "primary") {
                    "${Environment.getExternalStorageDirectory()}/${split[1]}"
                } else null
            }
            // Media Provider
            uri.authority == "com.android.providers.media.documents" -> {
                val split = docId.split(":")
                val type = split[0]
                val contentUri = when (type) {
                    "image" -> MediaStore.Images.Media.EXTERNAL_CONTENT_URI
                    else -> null
                }
                contentUri?.let {
                    val selection = "_id=?"
                    val selectionArgs = arrayOf(split[1])
                    getPathFromMediaStore(ContentUris.withAppendedId(it, split[1].toLong()))
                }
            }
            else -> copyUriToTempFile(uri)
        }
    }

    /// @brief URIの内容を一時ファイルにコピーします
    private fun copyUriToTempFile(uri: Uri): String? {
        return try {
            val inputStream: InputStream = contentResolver.openInputStream(uri) ?: return null
            
            // 一時ファイルを作成
            val tempFile = File.createTempFile("siv3d_selected_", ".tmp", cacheDir)
            val outputStream = FileOutputStream(tempFile)
            
            // データをコピー
            inputStream.copyTo(outputStream)
            inputStream.close()
            outputStream.close()
            
            Log.d(TAG, "File copied to temp: ${tempFile.absolutePath}")
            tempFile.absolutePath
        } catch (e: Exception) {
            Log.e(TAG, "Error copying URI to temp file", e)
            null
        }
    }





    /// @brief カメラ用バックグラウンドスレッドを開始します
    private fun startBackgroundThread() {
        backgroundThread = HandlerThread("CameraBackground")
        backgroundThread?.start()
        backgroundHandler = Handler(backgroundThread!!.looper)
    }

    /// @brief カメラ用バックグラウンドスレッドを停止します
    private fun stopBackgroundThread() {
        backgroundThread?.let { thread ->
            thread.quitSafely()
            try {
                thread.join()
                backgroundThread = null
                backgroundHandler = null
            } catch (e: InterruptedException) {
                Log.e(TAG, "Error stopping background thread", e)
            }
        }
    }

    /// @brief カメラ権限をリクエストします
    private fun requestCameraPermission() {
        if (ActivityCompat.shouldShowRequestPermissionRationale(this, Manifest.permission.CAMERA)) {
            // 理由を表示してから権限をリクエスト
            runOnUiThread {
                Toast.makeText(this, "Camera permission is needed for this app", Toast.LENGTH_SHORT).show()
                ActivityCompat.requestPermissions(
                    this,
                    arrayOf(Manifest.permission.CAMERA),
                    CAMERA_PERMISSION_REQUEST_CODE
                )
            }
        } else {
            ActivityCompat.requestPermissions(
                this,
                arrayOf(Manifest.permission.CAMERA),
                CAMERA_PERMISSION_REQUEST_CODE
            )
        }
    }

    /// @brief 最適なカメラを選択します
    /// @param manager カメラマネージャー
    /// @return 選択されたカメラID（見つからない場合はnull）
    private fun chooseBestCamera(manager: CameraManager): String? {
        return try {
            val cameraIds = manager.cameraIdList
            Log.d(TAG, "Available camera IDs: ${cameraIds.joinToString()}")

            if (cameraIds.isEmpty()) {
                Log.w(TAG, "No camera IDs found")
                return null
            }

            // 各カメラの情報をログ出力
            for (cameraId in cameraIds) {
                try {
                    val characteristics = manager.getCameraCharacteristics(cameraId)
                    val facing = characteristics.get(CameraCharacteristics.LENS_FACING)
                    val facingStr = when (facing) {
                        CameraCharacteristics.LENS_FACING_BACK -> "BACK"
                        CameraCharacteristics.LENS_FACING_FRONT -> "FRONT"
                        CameraCharacteristics.LENS_FACING_EXTERNAL -> "EXTERNAL"
                        else -> "UNKNOWN($facing)"
                    }
                    Log.d(TAG, "Camera $cameraId: facing=$facingStr")
                } catch (e: Exception) {
                    Log.w(TAG, "Error getting characteristics for camera $cameraId", e)
                }
            }

            // 背面カメラを優先
            for (cameraId in cameraIds) {
                try {
                    val characteristics = manager.getCameraCharacteristics(cameraId)
                    val facing = characteristics.get(CameraCharacteristics.LENS_FACING)
                    if (facing != null && facing == CameraCharacteristics.LENS_FACING_BACK) {
                        Log.i(TAG, "Selected back camera: $cameraId")
                        return cameraId
                    }
                } catch (e: Exception) {
                    Log.w(TAG, "Error checking camera $cameraId for back facing", e)
                    continue
                }
            }

            // 背面カメラがない場合は前面カメラを探す
            for (cameraId in cameraIds) {
                try {
                    val characteristics = manager.getCameraCharacteristics(cameraId)
                    val facing = characteristics.get(CameraCharacteristics.LENS_FACING)
                    if (facing != null && facing == CameraCharacteristics.LENS_FACING_FRONT) {
                        Log.i(TAG, "Selected front camera: $cameraId")
                        return cameraId
                    }
                } catch (e: Exception) {
                    Log.w(TAG, "Error checking camera $cameraId for front facing", e)
                    continue
                }
            }

            // どちらもない場合は最初の利用可能なカメラを使用
            val firstCamera = cameraIds[0]
            Log.i(TAG, "Selected first available camera: $firstCamera")
            return firstCamera

        } catch (e: Exception) {
            Log.e(TAG, "Error getting camera list", e)
            return null
        }
    }

    /// @brief カメラの特性を設定します
    /// @param characteristics カメラの特性
    /// @return 設定が成功した場合はtrue
    private fun setupCameraCharacteristics(characteristics: CameraCharacteristics): Boolean {
        return try {
            // フラッシュサポートを確認
            val available = characteristics.get(CameraCharacteristics.FLASH_INFO_AVAILABLE)
            // isFlashSupported = available ?: false // Removed Camera2 related checks
            Log.d(TAG, "Flash supported: $available")

            // 利用可能な出力サイズを取得して最適なものを選択
            val streamConfigMap = characteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP)
            if (streamConfigMap == null) {
                Log.e(TAG, "Stream configuration map is null")
                return false
            }

            val outputSizes = streamConfigMap.getOutputSizes(ImageFormat.YUV_420_888)
            if (outputSizes == null || outputSizes.isEmpty()) {
                Log.e(TAG, "No YUV_420_888 output sizes available")
                return false
            }

            // previewSize = chooseOptimalSize(outputSizes, MAX_PREVIEW_WIDTH, MAX_PREVIEW_HEIGHT) // Removed Camera2 related checks
            Log.i(TAG, "Available output sizes: ${outputSizes.joinToString { "${it.width}x${it.height}" }}}")

            // previewSize != null // Removed Camera2 related checks
            true
        } catch (e: Exception) {
            Log.e(TAG, "Error setting up camera characteristics", e)
            false
        }
    }

    /// @brief 最適なサイズを選択します
    /// @param choices 選択可能なサイズの配列
    /// @param maxWidth 最大幅
    /// @param maxHeight 最大高さ
    /// @return 選択されたサイズ
    private fun chooseOptimalSize(choices: Array<Size>, maxWidth: Int, maxHeight: Int): Size {
        // 優先候補（解像度を揃えたいターゲット）
        val preferred = arrayOf(
            Size(1280, 720),
            Size(1920, 1080),
            Size(960, 540),
            Size(854, 480)
        )
        // 1) 完全一致を探す（max 範囲内）
        preferred.firstOrNull { p ->
            choices.any { it.width == p.width && it.height == p.height && it.width <= maxWidth && it.height <= maxHeight }
        }?.let { p -> return p }

        // 2) 16:9 の中から最大（max 範囲内）
        val sixteenNine = choices.filter { it.width <= maxWidth && it.height <= maxHeight }
            .filter {
                val r = it.width.toDouble() / it.height.toDouble()
                kotlin.math.abs(r - 16.0 / 9.0) < 0.03
            }
        if (sixteenNine.isNotEmpty()) {
            return sixteenNine.maxBy { it.width * it.height }
        }

        // 3) それ以外の中で最大（max 範囲内）
        val underMax = choices.filter { it.width <= maxWidth && it.height <= maxHeight }
        if (underMax.isNotEmpty()) {
            return underMax.maxBy { it.width * it.height }
        }

        // 4) フォールバック
        return choices[0]
    }

    /// @brief ImageReaderを設定します
    /// @return 設定が成功した場合はtrue
    private fun setupImageReader(): Boolean {
        return try {
            Log.d(TAG, "ImageReader setup complete")
            true
        } catch (e: Exception) {
            Log.e(TAG, "Error setting up ImageReader", e)
            false
        }
    }

    /// @brief カメラを閉じます
    private fun closeCamera() {
        try {
        } catch (e: InterruptedException) {
            throw RuntimeException("Interrupted while trying to lock camera closing.", e)
        } finally {
            // cameraOpenCloseLock.release() // Removed Camera2 related checks
        }
    }

    /// @brief カメラデバイスの状態コールバック
    private val stateCallback = object : CameraDevice.StateCallback() {
        override fun onOpened(camera: CameraDevice) {
            Log.i(TAG, "Camera opened successfully")
        }

        override fun onDisconnected(camera: CameraDevice) {
            camera.close()
            Log.w(TAG, "Camera disconnected")
        }

        override fun onError(camera: CameraDevice, error: Int) {
            camera.close()
            Log.e(TAG, "Camera error: $error")
        }
    }

    /// @brief カメラキャプチャセッションを作成します
    private fun createCameraCaptureSession() {
        try {
        } catch (e: CameraAccessException) {
            Log.e(TAG, "Failed to create capture session", e)
        }
    }

    /// @brief イメージが利用可能になったときのリスナー
    private val onImageAvailableListener = ImageReader.OnImageAvailableListener { reader ->
        var image: Image? = null
        try {
            image = reader.acquireLatestImage()
            if (image == null) {
                return@OnImageAvailableListener
            }

            if (image.format != ImageFormat.YUV_420_888) {
                Log.w(TAG, "Unexpected image format: ${image.format}")
                return@OnImageAvailableListener
            }

            val planes = image.planes
            if (planes.size < 3) {
                Log.w(TAG, "Insufficient planes: ${planes.size}")
                return@OnImageAvailableListener
            }

            val yBuffer = planes[0].buffer
            val uBuffer = planes[1].buffer
            val vBuffer = planes[2].buffer

            // バッファサイズを検証
            if (yBuffer == null || uBuffer == null || vBuffer == null) {
                Log.w(TAG, "Null buffer detected")
                return@OnImageAvailableListener
            }
        } catch (e: Exception) {
            Log.e(TAG, "Error processing image", e)
        } finally {
            image?.close()
        }
    }

    /// @brief エラーメッセージを表示します
    /// @param message 表示するメッセージ
    private fun showError(message: String) {
        runOnUiThread {
            Toast.makeText(this, "Camera Error: $message", Toast.LENGTH_LONG).show()
        }
    }

    /// @brief 権限リクエストの結果を処理します
    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == CAMERA_PERMISSION_REQUEST_CODE) {
            if (grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
            } else {
                showError("Camera permission denied")
                Log.e(TAG, "Camera permission was denied.")
            }
        }
    }

    /// BTマウス/トラックパッド入力を処理
    override fun onGenericMotionEvent(event: MotionEvent): Boolean {
        if (event.source and InputDevice.SOURCE_MOUSE == InputDevice.SOURCE_MOUSE) {
            when (event.action) {
                MotionEvent.ACTION_HOVER_MOVE -> {
                    // BTマウスは常に指0として処理
                    val x = event.x.toInt()
                    val y = event.y.toInt()
                    Log.i("BTMouse", "Cursor move: ($x, $y)")
                    onCursorUpdateNative(x, y)
    
                    // ID0としてのMOVEイベントも送信（VPadでは無視される）
                    onMultiTouchEventNative(ACTION_MOVE, 0, x, y)
                    return true
                }
                MotionEvent.ACTION_BUTTON_PRESS,
                MotionEvent.ACTION_BUTTON_RELEASE -> {
                    onMouseButtonNative(event.buttonState)
                    return true
                }
            }
        }
        return super.onGenericMotionEvent(event)
    }

















}