# include <Siv3D.hpp> // OpenSiv3D v0.6.11

void Main()
{
	Scene::SetBackground(ColorF{ 0.8, 0.9, 1.0 });

	constexpr StringView ButtonText[]
	{
		U"Iris In",
		U"Iris Out",
	};

	const Array<String> options
	{
		U"\u25cf",
		U"\u2665",
		U"\u2605",
	};

	const Polygon masks[]
	{
		Circle{ 0, 0, 500 }.asPolygon(),
		Shape2D::Heart(700),
		Shape2D::Star(1100),
	};

	const MSRenderTexture renderTexture{ Scene::Size() };

	constexpr double TransitionTime = 1;  // 秒

	bool hidden = false;
	bool irising = false;
	double irisTime = 0;
	std::size_t radioIndex = 0;

	const Texture texture1{ U"example/windmill.png" };
	const Texture texture2{ U"example/siv3d-kun.png" };

	const auto draw = [&]
	{
		texture1.draw(40, 20);
		texture2.draw(400, 100);
	};

	while (System::Update())
	{
		if (irising)
		{
			irisTime += (Scene::DeltaTime() * (hidden ? 1 : -1)) / TransitionTime;
			if (irisTime <= 0 || 1 <= irisTime)
			{
				irising = false;
				hidden = not hidden;
			}
		}

		if (irising)
		{
			{
				const ScopedRenderTarget2D target{ renderTexture.clear(Scene::GetBackground()) };

				draw();
			}

			Graphics2D::Flush();
			renderTexture.resolve();

			Scene::Rect().draw(Palette::Black);
			masks[radioIndex].scaled(irisTime).draw(Scene::CenterF(), Palette::White);

			{
				const ScopedRenderStates2D blend{ BlendState::Multiplicative };

				renderTexture.draw();
			}
		}
		else if (hidden)
		{
			Scene::Rect().draw(Palette::Black);
		}
		else
		{
			draw();
		}

		if (SimpleGUI::Button(ButtonText[hidden ? 0 : 1], Vec2{ 680, 20 }, 100, not irising))
		{
			irising = true;
			irisTime = (hidden ? 0 : 1);
		}

		SimpleGUI::RadioButtons(radioIndex, options, Vec2{ 680, 70 }, 100, not irising);
	}
}
