# include "DeviceOrientation.hpp"

# include <emscripten.h>

namespace DeviceOrientation
{
	namespace detail
	{
		EM_JS
		(
			void, DeviceOrientation_RequestPermission, (),
			{
				if (DeviceOrientationEvent.requestPermission) {
					(async () => {
						const permission = await DeviceOrientationEvent.requestPermission();
						if (permission === "granted") {
							siv3dDeviceOrientation.setEvent();
						}
						siv3dDeviceOrientation.permission = permission;
					})();
				}
			}
		);
		
		EM_JS
		(
			PermissionStatus, DeviceOrientation_GetPermission, (),
			{
				return ["default", "granted", "denied"].indexOf(siv3dDeviceOrientation.permission);
			}
		);

		EM_JS
		(
			void, DeviceOrientation_GetOrientation, (double* pAlpha, double* pBeta, double* pGamma),
			{
				if (typeof(siv3dDeviceOrientation.event.alpha) === "number") {
					const { alpha, beta, gamma } = siv3dDeviceOrientation.event;
					setValue(pAlpha, alpha * (Math.PI / 180), "double");
					setValue(pBeta, beta * (Math.PI / 180), "double");
					setValue(pGamma, gamma * (Math.PI / 180), "double");
				}
			}
		);

		EM_JS
		(
			bool, DeviceOrientation_IsAbsolute, (),
			{
				return (siv3dDeviceOrientation.event.absolute ?? false);
			}
		);
	}

	void RequestPermission()
	{
		detail::DeviceOrientation_RequestPermission();
	}

	PermissionStatus Permission()
	{
		return detail::DeviceOrientation_GetPermission();
	}

	Rotation Orientation()
	{
		Rotation orientation{};

		detail::DeviceOrientation_GetOrientation(&orientation.alpha, &orientation.beta, &orientation.gamma);

		return orientation;
	}

	bool IsAbsolute()
	{
		return detail::DeviceOrientation_IsAbsolute();
	}
}
