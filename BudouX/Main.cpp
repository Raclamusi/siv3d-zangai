# include <Siv3D.hpp> // OpenSiv3D v0.6.11

class BudouXParser
{
public:

	SIV3D_NODISCARD_CXX20
	BudouXParser(HashTable<String, HashTable<String, int32>> model, Optional<int32> totalScore = none)
		: m_totalScore{ totalScore.value_or(0) }
		, m_model{ std::move(model) }
	{
		if (not totalScore)
		{
			for (const auto& [featureKey, group] : m_model)
			{
				for (const auto& [sequence, score] : group)
				{
					m_totalScore += score;
				}
			}
		}
	}

	[[nodiscard]]
	explicit operator bool() const
	{
		return (not m_model.empty());
	}

	[[nodiscard]]
	int32 getScore(StringView featureKey, StringView sequence) const
	{
		if (const auto itGroup = m_model.find(featureKey); itGroup != m_model.end())
		{
			const auto& group = itGroup->second;

			if (const auto itScore = group.find(sequence); itScore != group.end())
			{
				const auto& score = itScore->second;

				return score;
			}
		}

		return 0;
	}

	[[nodiscard]]
	Array<size_t> parseBoundaries(StringView sentence) const
	{
		constexpr std::tuple<StringView, int32, int32> Features[]
		{
			{ U"UW1", -3, 1 },
			{ U"UW2", -2, 1 },
			{ U"UW3", -1, 1 },
			{ U"UW4",  0, 1 },
			{ U"UW5",  1, 1 },
			{ U"UW6",  2, 1 },
			{ U"BW1", -2, 2 },
			{ U"BW2", -1, 2 },
			{ U"BW3",  0, 2 },
			{ U"TW1", -3, 3 },
			{ U"TW2", -2, 3 },
			{ U"TW3", -1, 3 },
			{ U"TW4",  0, 3 },
		};

		Array<size_t> result;

		for (int64 i = 1; i < static_cast<int64>(sentence.size()); ++i)
		{
			int32 score = 0;

			for (const auto& [key, pos, n] : Features)
			{
				if ((0 <= (i + pos)) && ((i + pos) < static_cast<int64>(sentence.size())))
				{
					score += getScore(key, sentence.substr((i + pos), n));
				}
			}

			if (score * 2 > m_totalScore)
			{
				result.push_back(i);
			}
		}
		return result;
	}

	[[nodiscard]]
	Array<String> parse(StringView sentence) const
	{
		Array<String> result;

		size_t start = 0;

		for (size_t boundary : parseBoundaries(sentence))
		{
			result.emplace_back(sentence.substr(start, (boundary - start)));

			start = boundary;
		}

		result.emplace_back(sentence.substr(start));

		return result;
	}

	[[nodiscard]]
	Array<StringView> parseView(StringView sentence) const
	{
		Array<StringView> result;

		size_t start = 0;

		for (size_t boundary : parseBoundaries(sentence))
		{
			result.push_back(sentence.substr(start, (boundary - start)));

			start = boundary;
		}

		result.push_back(sentence.substr(start));

		return result;
	}

	template <class Reader, std::enable_if_t<std::is_base_of_v<IReader, Reader>>* = nullptr>
	[[nodiscard]]
	static BudouXParser Load(Reader&& reader)
	{
		HashTable<String, HashTable<String, int32>> model;
		int32 totalScore = 0;

		const auto modelJSON = JSON::Load(std::forward<Reader>(reader));

		for (const auto& [featureKey, groupJSON] : modelJSON)
		{
			auto& group = model[featureKey];

			for (const auto& [sequence, scoreJSON] : groupJSON)
			{
				const int32 score = scoreJSON.getOr<int32>(0);

				group[sequence] = score;

				totalScore += score;
			}
		}

		return BudouXParser{ std::move(model), totalScore };
	}

	[[nodiscard]]
	static BudouXParser Load(FilePathView path)
	{
		return Load(BinaryReader{ path });
	}

	[[nodiscard]]
	static BudouXParser Download(URLView url)
	{
		MemoryWriter writer;

		SimpleHTTP::Get(url, {}, writer);

		return Load(MemoryReader{ writer.retrieve() });
	}

private:

	int32 m_totalScore;

	HashTable<String, HashTable<String, int32>> m_model;
};

void Main()
{
	const auto parser = BudouXParser::Download(U"https://raw.githubusercontent.com/google/budoux/main/budoux/models/ja.json");

	const Font font{ FontMethod::MSDF, 48 };

	TextAreaEditState textAreaState
	{
		U"Siv3D（シブスリーディー）は、音や画像、AI を使ったゲームやアプリを、"
		U"モダンな C++ コードで楽しく簡単にプログラミングできるオープンソースのフレームワークです。"
	};

	auto parsedText = parser.parse(textAreaState.text);

	double fontSizeSlider = 0.4;

	bool forceReturn = false;

	while (System::Update())
	{
		if (SimpleGUI::TextArea(textAreaState, Vec2{ 30, 20 }, SizeF{ 740, 100 }))
		{
			parsedText = parser.parse(textAreaState.text.removed(U'\n'));
		}

		SimpleGUI::Slider(U"Font size", fontSizeSlider, Vec2{ 30, 130 }, 100, 200);
		const double fontSize = (fontSizeSlider * 80 + 16);

		SimpleGUI::CheckBox(forceReturn, U"境界で必ず改行する", Vec2{ 340, 130 });

		{
			Vec2 pos{ 30, 180 };

			for (const auto& s : parsedText)
			{
				const auto text = font(s);
				const auto advances = text.getXAdvances(fontSize);

				if (pos.x != 30 && (forceReturn || ((pos.x + advances.sum()) > 770)))
				{
					pos.x = 30;
					pos.y += font.height(fontSize);
				}

				double advance = 0;
				size_t start = 0;
				for (size_t i = 0; i < text.clusters.size(); ++i)
				{
					advance += advances[i];
					if ((pos.x + advance) > 770)
					{
						font(s.substr(start, (text.clusters[i].pos - start))).draw(fontSize, pos);
						pos.x = 30;
						pos.y += font.height(fontSize);
						advance = advances[i];
						start += text.clusters[i].pos;
					}
				}
				font(s.substr(start)).draw(fontSize, pos);
				pos.x += advance;
			}
		}
	}
}
