# include "LocalStorage.hpp"

# include <emscripten.h>

using namespace s3d;

namespace LocalStorage
{
	namespace detail
	{
		EM_JS
		(
			bool, Exists, (const char32* key),
			{
				return localStorage.getItem(UTF32ToString(key)) !== null;
			}
		);

		EM_JS
		(
			std::size_t, GetLength, (const char32* key),
			{
				return localStorage.getItem(UTF32ToString(key))?.length ?? -1;
			}
		);

		EM_JS
		(
			void, GetItem, (const char32* key, char32* s, std::size_t size),
			{
				const item = localStorage.getItem(UTF32ToString(key)) ?? "";
				stringToUTF32(item, s, size * 4);
			}
		);

		EM_JS
		(
			void, SetItem, (const char32* key, const char32* item),
			{
				localStorage.setItem(UTF32ToString(key), UTF32ToString(item));
			}
		);

		EM_JS
		(
			void, RemoveItem, (const char32* key),
			{
				localStorage.removeItem(UTF32ToString(key));
			}
		);

		EM_JS
		(
			std::size_t, Length, (),
			{
				return localStorage.length;
			}
		);

		EM_JS
		(
			std::size_t, KeyLength, (std::size_t index),
			{
				return localStorage.key(index)?.length ?? -1;
			}
		);

		EM_JS
		(
			void, Key, (std::size_t index, char32* s, std::size_t size),
			{
				const key = localStorage.key(index);
				stringToUTF32(item, s, size * 4);
			}
		);

		EM_JS
		(
			void, Clear, (),
			{
				localStorage.clear();
			}
		);
	}

	bool Exists(const String& key)
	{
		return detail::Exists(key.c_str());
	}

	Optional<String> GetItem(const String& key)
	{
		const auto size = detail::GetLength(key.c_str());
		if (size == std::size_t(-1))
		{
			return none;
		}
		String item;
		if (size)
		{
			item.resize(size + 1);
			detail::GetItem(key.c_str(), item.data(), item.size());
			item.resize(size);
		}
		return item;
	}

	void SetItem(const String& key, const String& item)
	{
		detail::SetItem(key.c_str(), item.c_str());
	}

	void RemoveItem(const String& key)
	{
		detail::RemoveItem(key.c_str());
	}

	std::size_t Length()
	{
		return detail::Length();
	}

	Optional<String> Key(std::size_t index)
	{
		const auto size = detail::KeyLength(index);
		if (size == std::size_t(-1))
		{
			return none;
		}
		String key;
		if (size)
		{
			key.resize(size + 1);
			detail::Key(index, key.data(), key.size());
			key.resize(size);
		}
		return key;
	}

	void Clear()
	{
		detail::Clear();
	}
}
