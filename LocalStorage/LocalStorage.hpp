# pragma once

# include <Siv3D/String.hpp>
# include <Siv3D/Optional.hpp>
# include <cstddef>

namespace LocalStorage
{
	/// @brief ローカルストレージにキーが存在するかを返します。
	/// @param key 確認するキー
	/// @return キーが存在する場合 true, それ以外の場合は false
	[[nodiscard]]
	bool Exists(const s3d::String& key);

	/// @brief ローカルストレージに保存されたキーに対応する値を返します。
	/// @param key 取得する値のキー
	/// @return キーに対応する値、キーが存在しない場合は none
	[[nodiscard]]
	s3d::Optional<s3d::String> GetItem(const s3d::String& key);

	/// @brief ローカルストレージにキーを追加し、キーに対して値を保存します。
	/// @remark キーが既に存在する場合は、そのキーを更新します。
	/// @param key 追加、更新するキー
	/// @param item 保存、更新する値
	void SetItem(const s3d::String& key, const s3d::String& item);

	/// @brief ローカルストレージに保存されたキーを削除します。
	/// @remark 指定したキーが存在しない場合、何もしません。
	/// @param key 削除するキー
	void RemoveItem(const s3d::String& key);

	/// @brief ローカルストレージに保存されているデータの数を返します。
	/// @return 保存されているデータ数
	[[nodiscard]]
	std::size_t Length();

	/// @brief ローカルストレージの指定したインデックス番目のキーを返します。
	/// @remark キーの順序はユーザエージェント依存です。
	/// @param index 0 から始まるインデックス
	/// @return インデックス番目のキー、キーが存在しない場合は none
	[[nodiscard]]
	s3d::Optional<s3d::String> Key(std::size_t index);

	/// @brief ローカルストレージに保存されているデータをすべて削除します。
	void Clear();
}
