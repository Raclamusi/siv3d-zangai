# include <Siv3D.hpp> // OpenSiv3D v0.6.11

Image ToGIFImage(const Image& image)
{
	return Image{ MemoryReader{ image.encode(ImageFormat::GIF) } };
}

Image DitherImage(const Image& image)
{
	const auto tempFilePath = FileSystem::UniqueFilePath();
	ScopeGuard guard{ [&] { FileSystem::Remove(tempFilePath); } };
	AnimatedGIFWriter{ tempFilePath, image.size(), Dither::Yes }.writeFrame(image);
	return Image{ tempFilePath };
}

void Main()
{
	Window::Resize(1200, 800);

	const Font font{ FontMethod::MSDF, 30 };

	Image baseImage{ U"example/bay.jpg" };
	Image gifImage = ToGIFImage(baseImage);
	Image ditheredImage = DitherImage(baseImage);
	
	Texture gifTexture{ gifImage };
	Texture ditheredTexture{ ditheredImage };
	
	while (System::Update())
	{
		gifTexture.resized(600).drawAt(300, 400);
		ditheredTexture.resized(600).drawAt(900, 400);

		if (SimpleGUI::ButtonAt(U"画像を開く", Vec2{ 110, 50 }))
		{
			if (auto image = Dialog::OpenImage())
			{
				baseImage = std::move(image);
				gifImage = ToGIFImage(baseImage);
				ditheredImage = DitherImage(baseImage);
				gifTexture = Texture{ gifImage };
				ditheredTexture = Texture{ ditheredImage };
			}
		}

		font(U"ディザなしGIF化").drawAt(250, 750);
		font(U"ディザありGIF化").drawAt(850, 750);

		if (SimpleGUI::ButtonAt(U"保存", Vec2{ 420, 750 }))
		{
			gifImage.saveWithDialog();
		}

		if (SimpleGUI::ButtonAt(U"保存", Vec2{ 1020, 750 }))
		{
			ditheredImage.saveWithDialog();
		}
	}
}
