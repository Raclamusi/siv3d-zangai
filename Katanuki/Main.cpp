# include <Siv3D.hpp> // OpenSiv3D v0.6.11

// lineString と line の交点がなければ none 、あれば最も line.begin に近い交点の座標とその交点が存在する lineString の線のインデックスのペアを返す
[[nodiscard]]
Optional<std::pair<Vec2, std::size_t>> IntersectFirstAt(const LineString& lineString, const Line& line)
{
	Optional<std::pair<Vec2, std::size_t>> result;
	for (auto i : step(lineString.size()))
	{
		const auto line2 = lineString.line(i, CloseRing::Yes);
		if (auto point = line.intersectsAt(line2))
		{
			if (point->hasNaN())
			{
				point = line2.intersects(line.begin)
					? line.begin
					: (line2.begin.manhattanDistanceFrom(line.begin) < line2.end.manhattanDistanceFrom(line.begin))
						? line2.begin
						: line2.end;
			}
			// 交点が line2.end のときは次の line2.begin なのでスルー
			if (*point != line2.end)
			{
				const auto d = point->manhattanDistanceFrom(line.begin);
				if (not result || d < result->first.manhattanDistanceFrom(line.begin))
				{
					result = std::pair{ *point, i };
				}
			}
		}
	}
	return result;
}

// polygon を、pos を通る線で2つに分割する
[[nodiscard]]
Optional<std::array<Polygon, 2>> Split(const Polygon& polygon, const Vec2& pos)
{
	// polygon に穴がないかチェック
	if (polygon.num_holes() > 0)
	{
		return none;
	}

	// pos が polygon の内部にあるかチェック
	const LineString outer{ polygon.outer() };
	if (not polygon.contains(pos) || outer.intersects(pos))
	{
		return none;
	}

	// 3回ランダムな角度に亀裂を入れ、最も短い亀裂を選ぶ
	Vec2 crackBegin{}, crackEnd{};
	std::size_t indexBegin{}, indexEnd{};
	double crackLength = Math::Inf;
	const auto length = polygon.boundingRect().size.manhattanLength();  // 交差判定に十分な長さ
	for ([[maybe_unused]] auto i : step(3))
	{
		const auto angle = RandomClosedOpen(0_deg, 360_deg);
		auto [point1, index1] = *IntersectFirstAt(outer, Line{ pos, Arg::angle = angle, length });
		auto [point2, index2] = *IntersectFirstAt(outer, Line{ pos, Arg::angle = angle + 180_deg, length });
		const auto d = point1.distanceFromSq(point2);
		if (d < crackLength)
		{
			crackBegin = point1;
			crackEnd = point2;
			indexBegin = ((index1 + 1) % outer.size());
			indexEnd = ((index2 + 1) % outer.size());
			crackLength = d;
		}
	}

	// 亀裂で分割する
	Array<Vec2> polygon1{ crackEnd, crackBegin };
	for (auto i = indexBegin; i != indexEnd; i = ((i + 1) % outer.size()))
	{
		polygon1.push_back(outer[i]);
	}
	polygon1.unique_consecutive();
	Array<Vec2> polygon2{ crackBegin, crackEnd };
	for (auto i = indexEnd; i != indexBegin; i = ((i + 1) % outer.size()))
	{
		polygon2.push_back(outer[i]);
	}
	polygon2.unique_consecutive();

	return std::array<Polygon, 2>{ Polygon{ polygon1 }, Polygon{ polygon2 } };
}

void Main()
{
	Scene::SetBackground(ColorF{ 0.8, 0.9, 1.0 });

	std::list<Polygon> polygons{ Shape2D::Heart(180).asPolygon().movedBy(Scene::CenterF()) };
	bool dragging = false;

	while (System::Update())
	{
		if (MouseL.down())
		{
			const auto it = std::find_if(polygons.rbegin(), polygons.rend(), [](auto&& polygon) { return polygon.contains(Cursor::PosF()); });
			if (it != polygons.rend())
			{
				polygons.splice(polygons.end(), polygons, std::prev(it.base()));
			}
		}
		else if (MouseL.pressed())
		{
			if (const auto cursorDelta = Cursor::DeltaF(); not cursorDelta.isZero())
			{
				dragging = true;
				if (polygons.size())
				{
					polygons.back().moveBy(cursorDelta);
				}
			}
		}
		else if (MouseL.up())
		{
			if (not dragging && polygons.size())
			{
				if (auto ps = Split(polygons.back(), Cursor::PosF()))
				{
					polygons.pop_back();
					auto&& [p1, p2] = *ps;

					// クリックされた点を中心にちょっとだけ動かす
					p1.moveBy((p1.centroid() - Cursor::PosF()) * 0.2);
					p2.moveBy((p2.centroid() - Cursor::PosF()) * 0.2);

					polygons.push_back(std::move(p1));
					polygons.push_back(std::move(p2));
				}
			}
			dragging = false;
		}

		for (auto&& polygon : polygons)
		{
			polygon.draw(Palette::Pink);
			polygon.drawFrame(1, ColorF{ Palette::Black, 0.1 });
		}

		if (SimpleGUI::Button(U"全消去", Vec2{ 680, 20 }, 100))
		{
			polygons.clear();
		}

		if (SimpleGUI::Button(U"\u2605 追加", Vec2{ 680, 70 }, 100))
		{
			polygons.push_back(Shape2D::Star(180).asPolygon().movedBy(Scene::CenterF() + RandomVec2(50)));
		}

		if (SimpleGUI::Button(U"\u2665 追加", Vec2{ 680, 120 }, 100))
		{
			polygons.push_back(Shape2D::Heart(180).asPolygon().movedBy(Scene::CenterF() + RandomVec2(50)));
		}

		if (SimpleGUI::Button(U"\u25cf 追加", Vec2{ 680, 170 }, 100))
		{
			polygons.push_back(Circle{ 150 }.asPolygon().movedBy(Scene::CenterF() + RandomVec2(50)));
		}
	}
}
