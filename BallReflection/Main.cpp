# include <Siv3D.hpp> // OpenSiv3D v0.6.11

constexpr double BallSpeed = 300;

constexpr double BallRadius = 10;

constexpr ColorF LineColors[3]
{
	ColorF{ 1.0, 0.6, 0.6 },
	ColorF{ 1.0, 0.8, 0.8 },
	ColorF{ 1.0, 1.0, 1.0 },
};

[[nodiscard]]
Optional<Vec2> IntersectAt(const Line& line1, const Line& line2)
{
	if (const auto point = line1.intersectsAtPrecise(line2); point && not point->hasNaN())
	{
		// 若干ずれるので line2 上に収める
		return (line2.begin + (*point - line2.begin).projection(line2.vector()));
	}
	return none;
}

void Main()
{
	const Font font{ 200, Typeface::Bold };

	uint64 count = 0;

	double appleAppearanceTime = 0;
	Array<Circle> apples;

	const auto sceneRect = Scene::Rect();
	Array<std::pair<Line, int32>> walls
	{
		{ sceneRect.top(), -1 },
		{ sceneRect.left(), -1 },
		{ sceneRect.right(), -1 },
		{ sceneRect.bottom(), -1 },
	};

	Circle ball{ Scene::CenterF(), BallRadius };
	double ballVelocityAngle = 45_deg;

	Optional<Vec2> dragStart;

	while (System::Update())
	{
		// アップルの更新
		{
			if (appleAppearanceTime <= Scene::Time())
			{
				apples.emplace_back(RandomVec2(sceneRect), BallRadius * 2);
				appleAppearanceTime += Random(5.0, 10.0);
			}
		}

		// 線の更新
		{
			if (MouseL.down())
			{
				dragStart = Cursor::PosF();
			}
			if (MouseL.up())
			{
				if (dragStart)
				{
					const auto dragEnd = Cursor::PosF();
					if ((dragEnd - *dragStart).lengthSq() >= Pow(BallRadius * 2, 2))
					{
						walls.emplace_back(Line{ *dragStart, dragEnd }, static_cast<int32>(std::size(LineColors)) - 1);
					}
					dragStart = none;
				}
			}
		}

		// ボールの更新
		{
			auto speed = BallSpeed * Scene::DeltaTime();

			while (speed > 0)
			{
				const Line diff{ ball.center, Arg::angle = ballVelocityAngle, speed };

				// 衝突判定
				std::size_t index = walls.size();
				Vec2 point = diff.end;
				double distance = Math::Inf;
				for (auto&& [i, wall] : Indexed(walls))
				{
					auto&& [line, life] = wall;
					if (const auto p = IntersectAt(diff, line))
					{
						const auto d = p->manhattanDistanceFrom(diff.begin);
						if (1e-10 < d && d < distance)
						{
							index = i;
							point = *p;
							distance = d;
						}
					}
				}

				// アップル獲得判定、移動
				const auto numOfApples = apples.size();
				const Line trajectory{ diff.begin, point };
				apples.remove_if([&](auto&& apple)
				{
					return trajectory.intersects(apple);
				});
				count += (numOfApples - apples.size());
				ball.center = point;
				if (index == walls.size())
				{
					break;
				}

				// 反射
				auto&& [line, life] = walls[index];
				const auto incidence = diff.vector();
				const auto normal = line.normal();
				const auto reflection = incidence - 2 * Dot(incidence, normal) * normal;
				speed = point.distanceFrom(diff.end);
				ballVelocityAngle = reflection.getAngle();

				// 線の破壊
				if (life >= 0)
				{
					--life;
					if (life < 0)
					{
						walls.remove_at(index);
					}
				}
			}
		}

		// 描画
		{
			font(count).drawAt(Scene::Center(), ColorF{ Palette::White, 0.5 });

			for (auto&& apple : apples)
			{
				apple.draw(ColorF{ Palette::Lightgreen, 0.7 });
			}

			for (auto&& [line, life] : walls)
			{
				if (life >= 0)
				{
					line.draw(LineStyle::RoundCap, BallRadius * 2, LineColors[life]);
				}
			}

			ball.draw();

			if (dragStart)
			{
				Line{ *dragStart, Cursor::PosF() }.draw(LineStyle::RoundCap, BallRadius * 2, ColorF{ Palette::Lightblue, 0.7 });
			}
		}
	}
}
