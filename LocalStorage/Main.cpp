# include <Siv3D.hpp> // OpenSiv3D for Web v0.6.6r1
# include "LocalStorage.hpp"

void Main()
{
	const Font font{ 64 };

	uint64 count = 0;
	if (auto savedCount = LocalStorage::GetItem(U"count"))
	{
		if (auto value = ParseIntOpt<uint64>(*savedCount))
		{
			count = *value;
		}
	}

	String countString = ToString(count);

	while (System::Update())
	{
		font(countString).drawAt(Scene::Center());

		if (SimpleGUI::ButtonAt(U"+1", Scene::CenterF().movedBy(0, 100)))
		{
			++count;
			countString = ToString(count);
			LocalStorage::SetItem(U"count", countString);
		}
	}
}
