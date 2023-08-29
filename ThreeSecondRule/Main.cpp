# include <Siv3D.hpp> // OpenSiv3D v0.6.11

struct Food
{
	Food(const TextureRegion& texture, const Vec2& pos, const Vec2& from)
		: texture{ texture }
		, pos{ pos }
		, from{ from }
		, transition{ 0.2s, 0s }
		, remainingTime{ 3 }
	{
	}
		
	[[nodiscard]]
	bool isReady() const noexcept
	{
		return transition.isOne();
	}

	[[nodiscard]]
	bool isClean() const noexcept
	{
		return (remainingTime > 0);
	}

	[[nodiscard]]
	Vec2 getPos() const
	{
		return from.lerp(pos, transition.easeIn());
	}

	RectF getRegion() const
	{
		return texture.regionAt(getPos());
	}

	void move(const Vec2& delta)
	{
		from += delta;

		if (isReady())
		{
			from = pos;
			transition.reset();
		}
	}

	void update(const double deltaTime = Scene::DeltaTime())
	{
		if (isReady())
		{
			remainingTime = Max(0.0, remainingTime - deltaTime);
		}
		else
		{
			transition.update(true, deltaTime);
		}
	}

	void draw(const Font& font) const
	{
		const auto maskColor = (isClean() ? Palette::White : Palette::Lightgreen);

		texture.drawAt(getPos(), maskColor);

		if (isReady())
		{
			const auto gaugePos = pos.movedBy(-25, -35);

			Line{ gaugePos, Arg::direction(50, 0) }.draw(3, Palette::Red);
			Line{ gaugePos, Arg::direction(50 * remainingTime / 3, 0) }.draw(3, Palette::White);

			const auto textPos = pos.movedBy(0, -40);
			const auto textColor = (isClean() ? Palette::White : Palette::Red);

			font(U"{:.2f}"_fmt(remainingTime)).drawBaseAt(20, textPos, textColor);
		}
	}

private:
	TextureRegion texture;

	Vec2 pos;

	Vec2 from;

	Transition transition;

	double remainingTime;
};

void Main()
{
	const Font font{ FontMethod::MSDF, 48, Typeface::Bold };
	font.setBufferThickness(3);

	const Texture floor{ U"example/texture/wood.jpg" };

	constexpr Vec2 BentoPos{ 400, 550 };
	constexpr Vec2 MeatPos{ 150, 520 };
	constexpr Vec2 GermPos{ 650, 520 };

	const Texture bento{ U"🍱"_emoji };
	const Texture meat{ U"🍖"_emoji };
	const Texture germ{ U"🦠"_emoji };

	ColorF bentoColor = Palette::White;

	const Array<StringView> foodEmojis
	{
		U"🌽", U"🍅", U"🍉", U"🍌", U"🍓", U"🍔", U"🍗", U"🍙", U"🍠", U"🍡",
		U"🍢", U"🍣", U"🍤", U"🍥", U"🥓", U"🥝", U"🥟", U"🥦", U"🥪", U"🫑",
	};
	const auto foodTextures = foodEmojis.map([](auto&& emoji) { return Texture{ Emoji{ emoji } }; });

	// 3秒以上経ったおかずの数がこれに達するとゲームオーバー
	constexpr uint32 SpoiledFoodLimit = 5;

	uint32 returnCount = 0;

	double nextDropTime = 5;  // 秒

	bool isOver = false;

	Array<Food> droppedFoods;

	Optional<std::size_t> grabbedIndex;

	while (System::Update())
	{
		if (MouseL.up() && grabbedIndex)
		{
			auto&& food = droppedFoods[*grabbedIndex];

			if (bento.scaled(2).regionAt(BentoPos).contains(food.getPos()))
			{
				if (food.isClean())
				{
					++returnCount;
				}
				else
				{
					isOver = true;
					bentoColor = Palette::Lightgreen;
				}

				droppedFoods.remove_at(*grabbedIndex);
			}

			grabbedIndex = none;
		}

		if (grabbedIndex)
		{
			droppedFoods[*grabbedIndex].move(Cursor::DeltaF());
		}

		for (auto&& [i, food] : IndexedRef(droppedFoods))
		{
			if (i == grabbedIndex)
			{
				Cursor::RequestStyle(CursorStyle::Hand);
				continue;
			}

			food.update();

			if ((not isOver) && food.isReady() && food.getRegion().contains(Cursor::PosF()))
			{
				Cursor::RequestStyle(CursorStyle::Hand);

				if (MouseL.down())
				{
					grabbedIndex = i;
				}
			}
		}

		const auto spoiledFoodCount = droppedFoods.count_if([](auto&& food) { return (not food.isClean()); });

		if (spoiledFoodCount >= SpoiledFoodLimit)
		{
			isOver = true;
			grabbedIndex = none;
		}

		if ((not isOver) && (nextDropTime <= Scene::Time()))
		{
			const auto texture = foodTextures.choice();

			const auto pos = RandomVec2(RectF{ 60, 70, 680, 300 });

			droppedFoods.emplace_back(texture.scaled(0.5), pos, BentoPos);

			nextDropTime += (5.0 / (returnCount * 0.1 + 1));
		}

		floor.mapped(Scene::Size()).draw(ColorF{ 0.5 });

		bento.scaled(2).drawAt(BentoPos, bentoColor);

		Circle{ MeatPos, 50 }
			.draw(ColorF{ Palette::White, 0.3 })
			.drawFrame(10, Palette::White);

		meat.scaled(0.5).drawAt(MeatPos);

		font(returnCount).drawAt(TextStyle::Outline(0.2, Palette::Black), 50, MeatPos, Palette::White);

		Circle{ GermPos, 50 }
			.draw(ColorF{ Palette::White, 0.3 })
			.drawFrame(10, Palette::White)
			.drawArc(0, 360_deg * spoiledFoodCount / SpoiledFoodLimit, 5, 5, Palette::Red);

		germ.scaled(0.5).drawAt(GermPos);

		font(spoiledFoodCount).drawAt(TextStyle::Outline(0.2, Palette::Black), 50, GermPos, Palette::White);

		for (auto&& food : droppedFoods)
		{
			food.draw(font);
		}

		if (isOver)
		{
			Scene::Rect().draw(ColorF{ Palette::Black, 0.2 });

			font(U"GAME OVER").drawAt(TextStyle::Outline(0.2, Palette::Black), 100, Vec2{ 400, 230 }, Palette::White);

			font(U"SCORE: {}"_fmt(returnCount)).drawAt(TextStyle::Outline(0.2, Palette::Black), 50, Vec2{ 400, 330 }, Palette::White);

			if (SimpleGUI::ButtonAt(U"Restart", Vec2{ 400, 400 }))
			{
				bentoColor = Palette::White;
				returnCount = 0;
				nextDropTime = Scene::Time() + 5;
				isOver = false;
				droppedFoods.clear();
			}
		}
	}
}
