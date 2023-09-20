# pragma once

# include "ScreenOrientationType.hpp"

namespace ScreenOrientation
{
	/// @brief �����̌�����Ԃ��܂��B
	/// @return �����̌���
	[[nodiscard]]
	ScreenOrientationType Type();

	/// @brief �����̌����̊p�x��Ԃ��܂��B
	/// @return �����̌����̊p�x [rad]
	[[nodiscard]]
	double Angle();
}
