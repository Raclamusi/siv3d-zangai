# pragma once

# include <Siv3D/Types.hpp>

/// @brief 許可状態
enum class PermissionStatus : s3d::uint8
{
	/// @brief ユーザーの決定が分からない状態
	Default,

	/// @brief ユーザーが明示的な許可を与えている状態
	Granted,

	/// @brief ユーザーが明示的に拒否している状態
	Denied,
};
