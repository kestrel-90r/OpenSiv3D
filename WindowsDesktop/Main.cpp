SIV3D_SET(EngineOption::Renderer::OpenGL);

# include <Siv3D.hpp> // Siv3D v0.6.15

void Main()
{
	// 背景の色を設定する | Set the background color
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });


	// UDPクライアントを作成
	UDPClient receiver;
	// ポート8888で待ち受け
	if (!receiver.open(8888))
	{
		Print << U"Failed to open UDP receiver";
		return;
	}

	// 送信用のUDPクライアントを作成
	UDPClient sender;
	if (!sender.open(0)) // ポート0で自動割り当て
	{
		Print << U"Failed to open UDP sender";
		return;
	}

	// 送信メッセージの入力用テキストボックス
	TextEditState textEdit{ U"hello!UDP" };

	while (System::Update())
	{
		// GUIの描画
		SimpleGUI::TextBox(textEdit, Vec2{ 256, 20 }, 250);

		if (SimpleGUI::Button(U"Send", Vec2{ 512, 20 }))
		{
			// ボタンが押されたらメッセージを送信
			sender.send(IPv4Address::Localhost(), 8888,
				textEdit.text.toUTF8().data(),
				textEdit.text.toUTF8().size());
		}

		// 受信データがあれば表示
		if (receiver.available() > 0)
		{
			const size_t size = receiver.available();
			Array<Byte> buffer(size);

			if (receiver.read(buffer.data(), size))
		    {
				const std::string rxtext(reinterpret_cast<const char*>(buffer.data()), buffer.size());
				const String received = Unicode::FromUTF8(rxtext);
				Print << U"Received: " << received;
		    }
		}

	}
}

