# include <Siv3D.hpp> // OpenSiv3D v0.6.11

void SimplifyPolygonImpl(Array<Vec2>& vertices)
{
	auto size = vertices.size();
	if (size < 3)
	{
		return;
	}
	auto it1 = vertices.end() - 1;
	auto it2 = vertices.begin();
	auto it3 = vertices.begin() + 1;
	while (it2 != vertices.end())
	{
		const auto v1 = (*it2 - *it1);
		const auto v2 = (*it3 - *it2);
		const auto area = Abs(Cross(v1, v2)) / 2;
		if (area < 0.1)
		{
			*it2 = Vec2{ Math::QNaN, Math::QNaN };
			--size;
			if (size < 3)
			{
				break;
			}
		}
		else
		{
			it1 = it2;
		}
		++it2;
		do
		{
			++it3;
			if (it3 == vertices.end())
			{
				it3 = vertices.begin();
			}
		} while (it3->hasNaN());
	}
	vertices.remove_if([](auto&& v) { return v.hasNaN(); });
}

// P2World::createPolygon でエラーを出さないためのポリゴンの単純化
Polygon SimplifyPolygon(const Polygon& polygon)
{
	auto outer = polygon.outer();
	SimplifyPolygonImpl(outer);
	if (outer.size() < 3)
	{
		return Polygon{};
	}
	auto holes = polygon.inners();
	for (auto&& hole : holes)
	{
		SimplifyPolygonImpl(hole);
	}
	holes.remove_if([](auto&& hole) { return (hole.size() < 3); });
	return Polygon{ std::move(outer), std::move(holes) };
}

void Main()
{
	// 2D 物理演算のシミュレーションステップ（秒）
	constexpr double StepTime = (1.0 / 200.0);

	// 2D 物理演算のシミュレーション蓄積時間（秒）
	double accumulatedTime = 0.0;

	// 重力加速度 (cm/s^2)
	constexpr double Gravity = 500;

	// 2D 物理演算のワールド
	P2World world{ Gravity };

	// 枠
	const LineString frameLine{ Vec2{ 100, 50 }, Vec2{ 100, 550 }, Vec2{ 700, 550 }, Vec2{ 700, 50 } };
	const P2Body frame = world.createPolygon(P2Static, Vec2{ 0, 0 }, frameLine.calculateRoundBuffer(10));

	// 描いた線の密度 (kg/m^2)
	constexpr double LineDensity = 1.0;
	
	// Siv3D くんの密度 (kg/m^2)
	constexpr double Siv3DkunDensity = 0.2;
	
	// Siv3D くん
	auto siv3dkunPolygon = Image{ U"example/siv3d-kun.png" }.alphaToPolygonCentered().simplified();
	siv3dkunPolygon.scale(0.4);
	siv3dkunPolygon.moveBy(Scene::CenterF());
	const P2Body siv3dkun = world.createPolygon(P2Dynamic, Vec2{ 0, 0 }, siv3dkunPolygon, P2Material{ .density = Siv3DkunDensity });

	// 物体
	Array<P2Body> bodies;

	// 描いている途中の線
	LineString drawingLine;

	while (System::Update())
	{
		for (accumulatedTime += Scene::DeltaTime(); StepTime <= accumulatedTime; accumulatedTime -= StepTime)
		{
			// 2D 物理演算のワールドを更新する
			world.update(StepTime);
		}

		// 下に落ちた物体は削除する
		bodies.remove_if([](auto&& b) { return (2000 < b.getPos().y); });

		// 線を描く処理
		if (MouseL.down())
		{
			drawingLine.push_back(Cursor::PosF());
		}
		else if (MouseL.pressed())
		{
			if (Cursor::PosF().distanceFrom(drawingLine.back()) >= 10)
			{
				drawingLine.push_back(Cursor::PosF());
			}
		}
		else if (MouseL.up())
		{
			if (drawingLine.size() == 1)
			{
				bodies.push_back(world.createCircle(P2Dynamic, drawingLine[0], Circle{ 0, 0, 10 }, P2Material{ .density = LineDensity }));
			}
			else
			{
				if (auto polygon = SimplifyPolygon(drawingLine.calculateRoundBuffer(10)))
				{
					// 線の面積（概算） (cm^2)
					const double area = (drawingLine.calculateLength() * 20 + 100_pi);
					// 線の重量 (kg)
					const double weight = (LineDensity * area / 10000);
					// 線の見かけの面積 (cm^2)
					const double apparentArea = polygon.area();
					// 密度 (kg/m^2)
					const double density = (weight / apparentArea * 10000);

					const auto pos = polygon.centroid();
					polygon.moveBy(-pos);
					bodies.push_back(world.createPolygon(P2Dynamic, pos, polygon, P2Material{ .density = density }));
				}
			}
			drawingLine.clear();
		}

		// すべてのボディを描画する
		for (const auto& body : bodies)
		{
			body.draw();
		}

		// Siv3D くんを描画する
		siv3dkun.draw(Palette::Hotpink);

		// 地面を描画する
		frame.draw();

		// 描いている途中の線を描画する
		drawingLine.draw(LineStyle::RoundCap, 20, ColorF{ Palette::Blue, 0.7 });
	}
}
