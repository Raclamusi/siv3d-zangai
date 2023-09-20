# pragma once

# include <Siv3D/Types.hpp>

/// @brief 文書の向き
enum class ScreenOrientationType : s3d::uint8
{
	/// @brief 1つ目のポートレート（横長）モード
	PortraitPrimary,

	/// @brief 2つ目のポートレート（横長）モード
	PortraitSecondary,

	/// @brief 1つ目のランドスケープ（縦長）モード
	LandscapePrimary,

	/// @brief 2つ目のランドスケープ（縦長）モード
	LandscapeSecondary,
};
