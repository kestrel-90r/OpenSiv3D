<h1>Siv3D <a href="https://github.com/Siv3D/OpenSiv3D/blob/master/LICENSE"><img src="https://img.shields.io/badge/license-MIT-4aaa4a"></a> <a href="https://join.slack.com/t/siv3d/shared_invite/enQtNjM4NzQ0MzEyMzUzLTEzNDBkNWVkMTY0OGU5OWQxOTk3NjczMjk0OGJhYzJjOThjZjQ1YmYzMWU2NzQ5NTQ4ODg3NzE2ZmE0NmVlMTM"><img src="https://img.shields.io/badge/social-Slack-4a154b"></a> <a href="https://twitter.com/search?q=Siv3D%20OR%20OpenSiv3D&src=typed_query&f=live"><img src="https://img.shields.io/badge/social-Twitter-1DA1F2"></a> <a href="https://github.com/sponsors/Reputeless"><img src="https://img.shields.io/badge/funding-GitHub_Sponsors-ea4aaa"></a></h1>

<p align="center"><a href="https://siv3d.github.io/"><img src="https://raw.githubusercontent.com/Siv3D/File/master/v6/logo.png" width="480" alt="Siv3D logo"></a></p>

**Siv3D** (OpenSiv3D) is a C++20 framework for **creative coding** (2D/3D games, media art, visualizers, and simulators). Siv3D applications run on ** Windows, macOS, Linux, Web and the Android **.


## Main Features

- **Graphics**
  - Advanced 2D graphics
  - Basic 3D graphics (Wavefront OBJ, primitive shapes)
  - Custom vertex / pixel shaders ( GLSL ES )
  - Text rendering (Bitmap, SDF, MSDF)
  - PNG, JPEG, BMP, SVG, GIF, Animated GIF, TGA, PPM, WebP, TIFF
  - Unicode 14.0 emojis and 7,000+ icons
  - Image processing
  - Video rendering
- **Audio** 
  - WAVE, MP3, AAC, OggVorbis, Opus, MIDI, WMA*, FLAC*, AIFF*
  - Adjustable volume, pan, play speed and pitch
  - File streaming (WAVE, MP3, OggVorbis)
  - Fade in and fade out
  - Looping
  - Mixing busses
  - Filters (LPF, HPF, echo, reverb)
  - FFT
  - SoundFont rendering
  - Text to speech*
- **Input**
  - Mouse
  - Keyboard
  - ~~Gamepad~~
  - Webcam
  - Microphone
  - ~~Joy-Con / Pro Controller~~
  - ~~XInput*~~
  - Digital drawing tablet*
  - ~~Leap Motion*~~
- **Window**
  - Fullscreen mode
  - High DPI support
  - ~~Window styles (sizable, borderless)~~
  - ~~File dialog~~
  - ~~Drag & drop~~
  - ~~Message box~~
  - ~~Toast notification*~~
- **Network and communication**
  - HTTP client
  - Multiplayer (with Photon SDK)
  - TCP communication
  - Serial communication
  - Interprocess communication (pipe)
- **Math**
  - Vector and matrix classes (`Point`, `Float2`, `Vec2`, `Float3`, `Vec3`, `Float4`, `Vec4`, `Mat3x2`, `Mat3x3`, `Mat4x4`, `SIMD_Float4`, `Quaternion`)
  - 2D shape classes (`Line`, `Circle`, `Ellipse`, `Rect`, `RectF`, `Triangle`, `Quad`, `RoundRect`, `Polygon`, `MultiPolygon`, `LineString`, `Spline2D`, `Bezier2`, `Bezier3`)
  - 3D shape classes (`Plane`, `InfinitePlane`, `Sphere`, `Box`, `OrientedBox`, `Ray`, `Line3D`, `Triangle3D`, `ViewFrustum`, `Disc`, `Cylinder`, `Cone`)
  - Color classes (`Color`, `ColorF`, `HSV`)
  - Polar / cylindrical / spherical coordinates system
  - 2D / 3D shape intersection
  - 2D / 3D geometry processing
  - Rectangle packing
  - Planar subdivisions
  - Linear and gamma color space
  - Pseudo random number generators
  - Interpolation, easing, and smoothing
  - Perlin noise
  - Math parser
  - Navigation mesh
  - ~~Extended arithmetic types (`HalfFloat`, `int128`, `uint128`, `BigInt`, `BigFloat`)~~
- **String and Text Processing**
  - Advanced String class (`String`, `StringView`)
  - Unicode conversion
  - Regular expression
  - `{fmt}` style text formatting
  - Text reader / writer classes
  - CSV / INI / JSON / XML / TOML reader classes
  - CSV / INI / JSON writer classes
- **Misc**
  - Basic GUI (button, slider, radio buttons, checkbox, text box, color picker, list box)
  - Integrated 2D physics engine (Box2D)
  - Advanced array / 2D array classes (`Array`, `Grid`)
  - Kd-tree
  - Disjoint set
  - Asynchronous asset file streaming
  - Data compression (zlib, Zstandard)
  - Transitions between scenes
  - ~~File system~~
  - ~~Directory watcher~~
  - QR code reader / writer
  - GeoJSON
  - Date and time
  - Stopwatch and timer
  - Logging
  - Serialization
  - UUID
  - Child process
  - Clipboard
  - ~~Power status~~
  - ~~Scripting (AngelScript)~~

<small>* Some features are limited to specific platforms</small>


### ✨ v0.6.5
*released 31 Aug 2025*

| Platform   | Requirements                  |
|Android     | - Android 12.0+ (API level 31 or later)<br>- Android Studio 2025.1.2 or newer<br>- OpenGL ES 3.0+ compatible device |


## システム要件
Android 用 Siv3D をビルドするのに必要な開発環境は次のとおりです。

| 項目 | 必要環境 |
|:---|:---|
| OS | 64 ビット Windows 10 / macOS 12 / Linux ディストリビューション |
| CPU | Intel または AMD 製 CPU |
| 開発機 | Android 12 以降の端末 ※シミュレーションではテストできないため実機が必要 |
| 開発環境 | Android Studio 2025.1.2( https://developer.android.com/studio ) |


## コードを入手する
`Kestrel-90r` の Siv3D リポジトリから `forAndroid` ブランチをクローンしてください。

```bash
git clone -b forAndroid https://github.com/Kestrel-90r/OpenSiv3D.git
cd OpenSiv3D
```

## 開発言語について

Siv3D (Windows / macOS / Linux / Web) は **C++20** を標準としていますが、  
Android 版 Siv3D は **Android NDK** を使用するため、現在のサポート規格は **C++17** です。  

そのため、C++20 以降で導入された以下のような一部の機能は利用できません:

- `std::ranges` ライブラリ
- `std::format` （代わりに Siv3D の `Format` 関数や `{fmt}` を利用できます）
- `concepts`（`requires` や `concept` は使用不可）
- `std::span` の一部拡張
- 三方比較演算子 `<=>` など

## Android 版の注意点: コンテキストロストへの対応

Android では、**画面の消灯やアプリの一時停止・復帰**などの操作により  
OpenGL ES の描画コンテキストが頻繁に「ロスト（破棄）」されることがあります。  

デスクトップ版 Siv3D はコンテキストロストを想定していないため、  
通常のコードでは以下のような現象が起きます:

- 画面消灯 → 復帰後にアプリが停止する  

この対策のため、描画関係の初期化処理をbool Init()関数に記述する必要があります。
例えば、端末の画面消灯→復帰を行うと描画コンテキストが破棄されます。
復帰時にユーザーアプリ固有の初期化処理を再実行して、Textureやシェーダーなどのリソースを再構築します。
Android版Siv3Dでは、描画リソースをOptional<>の広域変数を使って管理します。

<details>
<summary>📄 Init()サンプルコード（クリックで展開）</summary>
```cpp
# include <Siv3D.hpp> // OpenSiv3D v0.6.5
SIV3D_SET(EngineOption::Renderer::OpenGLES)

Optional<Font> g_font ;
Optional<Texture> g_texture ;
Optional<Texture> g_emoji ;
Vec2 g_emojiPos{ 300, 150 };

// 初期化関数
bool Init()
{
    g_font.reset();
    g_texture.reset();
    g_emoji.reset();

    Window::Resize(800, 600);

    // 背景の色を設定 | Set background color
    Scene::SetBackground(ColorF{ 0.8, 0.9, 1.0 });

    // 通常のフォントを作成 | Create a new font
    g_font = Font{ 60 };

    // 絵文字用フォントを作成 | Create a new emoji font
    Font emojiFont = Font{ 60, Typeface::ColorEmoji };

    // `font` が絵文字用フォントも使えるようにする | Set emojiFont as a fallback
    g_font->addFallback( emojiFont );

    // 画像ファイルからテクスチャを作成 | Create a texture from an image file
    g_texture = Texture{ U"example/windmill.png" };

    // 絵文字からテクスチャを作成 | Create a texture from an emoji
    g_emoji = Texture{ U"🐈"_emoji };

    // 絵文字を描画する座標 | Coordinates of the emoji
    g_emojiPos = Vec2{ 300, 150 };

    return true;
}

void Main()
{
    // 初期化関数を呼び出し
    Init();

    // テキストを画面にデバッグ出力 | Print a text
    Print << U"Push [A] key";

    while (System::Update())
    {
        // テクスチャを描く | Draw a texture
        g_texture->draw(200, 200);

        // テキストを画面の中心に描く | Put a text in the middle of the screen
        (*g_font)(U"Hello, Siv3D!🚀").drawAt(Scene::Center(), Palette::Black);

        // サイズをアニメーションさせて絵文字を描く | Draw a texture with animated size
        g_emoji->resized(100 + Periodic::Sine0_1(1s) * 20).drawAt(g_emojiPos);

        // マウスカーソルに追随する半透明な円を描く | Draw a red transparent circle that follows the mouse cursor
        Circle{ Cursor::Pos(), 40 }.draw(ColorF{ 1, 0, 0, 0.5 });

        // もし [A] キーが押されたら | When [A] key is down
        if (KeyA.down())
        {
            // 選択肢からランダムに選ばれたメッセージをデバッグ表示 | Print a randomly selected text
            Print << Sample({ U"Hello!", U"こんにちは", U"你好", U"안녕하세요?" });
        }

        // もし [Button] が押されたら | When [Button] is pushed
        if (SimpleGUI::Button(U"Button", Vec2{ 640, 40 }))
        {
            // 画面内のランダムな場所に座標を移動
            // Move the coordinates to a random position in the screen
            g_emojiPos = RandomVec2(Scene::Rect());
        }
    }
}


```
</details> 



## Android 版の注意点: 入力デバイスの対応

デスクトップ版 Siv3D では **マウスやキーボード** を前提としていますが、  
スマートフォンにマウスやキーボードを接続して利用するケースは稀です。  

そのため Android 版では、**マウスやキーボードがなくても最低限の操作が可能**となるよう、  
仮想的な GUI 機能 **「VPad」** を用意しています。  

VPad は画面上に表示されるバーチャルコントローラで、以下のような操作を提供します:

- 画面タッチによる **マウスのボタンクリック相当の入力**
- 方向ボタンによる **カーソル移動やボタン入力の代替**
- 必要に応じて GUI ボタンを追加可能  

この仕組みにより、**マウスやキーボードを持たないスマートフォン環境でも、  
デスクトップ版と同様のアプリ操作が可能**になります。

<details>
<summary>📄 VPadサンプルコード（クリックで展開）</summary>
```cpp
# include <Siv3D.hpp> 
SIV3D_SET(EngineOption::Renderer::OpenGLES)

void DrawStick(const VPad *vpad,int16 vk, const Font& font, const Font& debugFont)
{
    if (vk != VKLSTICK && vk != VKRSTICK) return;
    
    auto stickInfo = vpad->GetStickInfo(vk);
    VPad::ButtonStyle style = vpad->GetButtonStyle(vk);
    
    // 基本円の描画
    stickInfo.baseCircle.scaled(1.2).draw(ColorF{0.3, 0.3, 0.3, 0.2});
    stickInfo.baseCircle.draw(ColorF{0.3, 0.3, 0.3, 0.3});
    stickInfo.baseCircle.drawFrame(2, ColorF{0.6, 0.6, 0.6, 0.8});
    Circle(stickInfo.baseCircle.center, stickInfo.baseCircle.r * 0.15).draw(ColorF{0.2, 0.2, 0.2, 0.2});
    
    // スティックつまみの描画
    stickInfo.knobCircle.draw(stickInfo.active ? style.activeColor : ColorF{0.5, 0.5, 0.7, 0.7});
    stickInfo.knobCircle.drawFrame(2, ColorF{1.0, 1.0, 1.0, 0.8});
    
    if (stickInfo.active)
        Line(stickInfo.baseCircle.center, stickInfo.knobCircle.center).draw(3, ColorF{1.0, 1.0, 1.0, 0.5});
    
    // ラベルの描画
    font(style.label).drawAt(stickInfo.baseCircle.center, ColorF{1.0, 1.0, 1.0, 0.8});
    
    // デバッグ情報表示（オプション）
    if (debugFont)
    {
        const String valueText = U"{:.2f},{:.2f}"_fmt(stickInfo.normalizedValue.x, stickInfo.normalizedValue.y);
        debugFont(valueText).draw(
            Arg::topCenter = Vec2{stickInfo.baseCircle.center.x, stickInfo.baseCircle.y - stickInfo.baseCircle.r - 30},
            ColorF{1.0, 1.0, 1.0, 0.8}
        );
    }
}

void Main()
{
    Window::Resize(1280, 720);
    
    auto* vpad = VPad::getInstance();
    vpad->Init();
    
    // ボタン領域登録
    const int btnY = vpad->AddRegion(RectF{1000, 300, 80, 80}, VKBTNY);
    const int btnX = vpad->AddRegion(RectF{900, 400, 80, 80}, VKBTNX);
    const int btnA = vpad->AddRegion(RectF{1000, 500, 80, 80}, VKBTNA);
    const int btnB = vpad->AddRegion(RectF{1100, 400, 80, 80}, VKBTNB);
    const int dpadUp    = vpad->AddRegion(RectF{ 200, 300, 80, 80}, VKUP);
    const int dpadLeft  = vpad->AddRegion(RectF{ 100, 400, 80, 80}, VKLEFT);
    const int dpadDown  = vpad->AddRegion(RectF{ 200, 500, 80, 80}, VKDOWN);
    const int dpadRight = vpad->AddRegion(RectF{ 300, 400, 80, 80}, VKRIGHT);
    const int btnR1 = vpad->AddRegion(RectF{100, 100, 200, 80}, VKR1);
    const int btnR2 = vpad->AddRegion(RectF{100, 200, 200, 80}, VKR2);
    const int btnL1 = vpad->AddRegion(RectF{1000, 100, 200, 80}, VKL1);
    const int btnL2 = vpad->AddRegion(RectF{1000, 200, 200, 80}, VKL2);
    const int btnStart  = vpad->AddRegion(RectF{600, 300, 100, 60}, VK_START);
    const int btnSelect = vpad->AddRegion(RectF{600, 400, 100, 60}, VKSELECT);
    const int stickL = vpad->AddRegion(RectF{420, 500, 180, 180}, VKLSTICK);
    const int stickR = vpad->AddRegion(RectF{680, 500, 180, 180}, VKRSTICK);
    const int btnLeft = vpad->AddRegion(RectF{50, 650, 80, 40}, VKBTNL);
    const int btnMiddle = vpad->AddRegion(RectF{150, 650, 80, 40}, VKBTNM);
    const int btnRight = vpad->AddRegion(RectF{250, 650, 80, 40}, VKBTNR);
    
    vpad->SetButtonStyle(VKBTNA, U"A", ColorF{0.9, 0.2, 0.2, 0.8}, ColorF{0.4, 0.1, 0.1, 0.5});
    vpad->SetButtonStyle(VKBTNB, U"B", ColorF{0.2, 0.9, 0.2, 0.8}, ColorF{0.1, 0.4, 0.1, 0.5});
    vpad->SetButtonStyle(VKBTNX, U"X", ColorF{0.2, 0.2, 0.9, 0.8}, ColorF{0.1, 0.1, 0.4, 0.5});
    vpad->SetButtonStyle(VKBTNY, U"Y", ColorF{0.9, 0.9, 0.2, 0.8}, ColorF{0.4, 0.4, 0.1, 0.5});
    vpad->SetButtonStyle(VKLEFT, U"←", ColorF{0.6, 0.6, 0.6, 0.8}, ColorF{0.3, 0.3, 0.3, 0.5});
    vpad->SetButtonStyle(VKRIGHT, U"→", ColorF{0.6, 0.6, 0.6, 0.8}, ColorF{0.3, 0.3, 0.3, 0.5});
    vpad->SetButtonStyle(VKUP, U"↑", ColorF{0.6, 0.6, 0.6, 0.8}, ColorF{0.3, 0.3, 0.3, 0.5});
    vpad->SetButtonStyle(VKDOWN, U"↓", ColorF{0.6, 0.6, 0.6, 0.8}, ColorF{0.3, 0.3, 0.3, 0.5});
    vpad->SetButtonStyle(VKL1, U"L1", ColorF{0.8, 0.5, 0.5, 0.8}, ColorF{0.4, 0.2, 0.2, 0.5});
    vpad->SetButtonStyle(VKL2, U"L2", ColorF{0.8, 0.3, 0.3, 0.8}, ColorF{0.4, 0.15, 0.15, 0.5});
    vpad->SetButtonStyle(VKR1, U"R1", ColorF{0.5, 0.8, 0.5, 0.8}, ColorF{0.2, 0.4, 0.2, 0.5});
    vpad->SetButtonStyle(VKR2, U"R2", ColorF{0.3, 0.8, 0.3, 0.8}, ColorF{0.15, 0.4, 0.15, 0.5});
    vpad->SetButtonStyle(VK_START, U"START", ColorF{0.7, 0.7, 0.7, 0.8}, ColorF{0.3, 0.3, 0.3, 0.5});
    vpad->SetButtonStyle(VKSELECT, U"SELECT", ColorF{0.7, 0.7, 0.7, 0.8}, ColorF{0.3, 0.3, 0.3, 0.5});
    vpad->SetButtonStyle(VKLSTICK, U"LS", ColorF{0.5, 0.5, 0.9, 0.8}, ColorF{0.2, 0.2, 0.4, 0.5});
    vpad->SetButtonStyle(VKRSTICK, U"RS", ColorF{0.5, 0.9, 0.5, 0.8}, ColorF{0.2, 0.4, 0.2, 0.5});
    vpad->SetButtonStyle(VKBTNL, U"LMB", ColorF{0.8, 0.2, 0.2, 0.8}, ColorF{0.4, 0.1, 0.1, 0.5});
    vpad->SetButtonStyle(VKBTNM, U"MMB", ColorF{0.2, 0.8, 0.2, 0.8}, ColorF{0.1, 0.4, 0.1, 0.5});
    vpad->SetButtonStyle(VKBTNR, U"RMB", ColorF{0.2, 0.2, 0.8, 0.8}, ColorF{0.1, 0.1, 0.4, 0.5});
    
    const Font font(32);
    const Font debugFont(24);
    
    while (System::Update())
    {
		if (MouseL.down()) Print << U"Left Click";
		if (MouseM.down()) Print << U"Middle Click";
		if (MouseR.down()) Print << U"Right Click";

        vpad->Update();
        
        // VPadの領域をループして描画
        for (const auto& region : vpad->GetRegions())
        {
            //アナログスティック    
            if (region.vk == VKLSTICK || region.vk == VKRSTICK)
            {
                DrawStick(vpad, region.vk, font, debugFont);
            }
 
            //ボタン
            else
            {
                RectF rect = vpad->GetButtonInfo(region.vk).rect;
                VPad::ButtonStyle style = vpad->GetButtonStyle(region.vk);

                bool isActive = vpad->IsButtonActive(region.vk);
                ColorF color = isActive ? style.activeColor : style.inactiveColor;
                rect.draw(color);
                rect.drawFrame(2, ColorF{1.0, 1.0, 1.0, 0.8});
                font(style.label).drawAt(rect.center(), ColorF{1.0});
            }
        }
    }
}
```
</details> 





