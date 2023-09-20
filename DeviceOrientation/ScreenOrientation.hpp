# pragma once

# include "ScreenOrientationType.hpp"

namespace ScreenOrientation
{
	/// @brief 文書の向きを返します。
	/// @return 文書の向き
	[[nodiscard]]
	ScreenOrientationType Type();

	/// @brief 文書の向きの角度を返します。
	/// @return 文書の向きの角度 [rad]
	[[nodiscard]]
	double Angle();
}
