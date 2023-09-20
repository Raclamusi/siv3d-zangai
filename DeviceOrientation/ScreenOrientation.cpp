# include "ScreenOrientation.hpp"

# include <emscripten.h>

namespace ScreenOrientation
{
	namespace detail
	{
		EM_JS
		(
			ScreenOrientationType, ScreenOrientation_GetType, (),
			{
				return ["portrait-primary", "portrait-secondary", "landscape-primary", "landscape-secondary"].indexOf(screen.orientation.type);
			}
		);

		EM_JS
		(
			double, ScreenOrientation_GetAngle, (),
			{
				return (window.orientation ?? screen.orientation.angle) * (Math.PI / 180);
			}
		);
	}

	ScreenOrientationType Type()
	{
		return detail::ScreenOrientation_GetType();
	}

	double Angle()
	{
		return detail::ScreenOrientation_GetAngle();
	}
}
