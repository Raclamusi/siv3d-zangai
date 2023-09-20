# include "DeviceMotion.hpp"

# include <emscripten.h>

using namespace s3d;

namespace DeviceMotion
{
	namespace detail
	{
		EM_JS
		(
			void, DeviceMotion_RequestPermission, (),
			{
				if (DeviceMotionEvent.requestPermission) {
					(async () => {
						const permission = await DeviceMotionEvent.requestPermission();
						if (permission === "granted") {
							siv3dDeviceMotion.setEvent();
						}
						siv3dDeviceMotion.permission = permission;
					})();
				}
			}
		);

		EM_JS
		(
			PermissionStatus, DeviceMotion_GetPermission, (),
			{
				return ["default", "granted", "denied"].indexOf(siv3dDeviceMotion.permission);
			}
		);

		EM_JS
		(
			void, DeviceMotion_GetAcceleration, (double* pX, double* pY, double* pZ),
			{
				if (siv3dDeviceMotion.event.acceleration) {
					const { x, y, z } = siv3dDeviceMotion.event.acceleration;
					const k = (siv3dDeviceMotion.is_iOS ? -1 : 1);
					setValue(pX, x * k, "double");
					setValue(pY, y * k, "double");
					setValue(pZ, z * k, "double");
				}
			}
		);

		EM_JS
		(
			void, DeviceMotion_GetAccelerationIncludingGravity, (double* pX, double* pY, double* pZ),
			{
				if (siv3dDeviceMotion.event.accelerationIncludingGravity) {
					const { x, y, z } = siv3dDeviceMotion.event.accelerationIncludingGravity;
					const k = (siv3dDeviceMotion.is_iOS ? -1 : 1);
					setValue(pX, x * k, "double");
					setValue(pY, y * k, "double");
					setValue(pZ, z * k, "double");
				}
			}
		);

		EM_JS
		(
			void, DeviceMotion_GetRotationRate, (double* pAlpha, double* pBeta, double* pGamma),
			{
				if (siv3dDeviceMotion.event.rotationRate) {
					const { alpha, beta, gamma } = siv3dDeviceMotion.event.rotationRate;
					setValue(pAlpha, alpha * (Math.PI / 180), "double");
					setValue(pBeta, beta * (Math.PI / 180), "double");
					setValue(pGamma, gamma * (Math.PI / 180), "double");
				}
			}
		);

		EM_JS
		(
			double, DeviceMotion_GetInterval, (),
			{
				return (siv3dDeviceMotion.event.interval ?? 0) / 1000;
			}
		);
	}

	void RequestPermission()
	{
		detail::DeviceMotion_RequestPermission();
	}

	PermissionStatus Permission()
	{
		return detail::DeviceMotion_GetPermission();
	}

	Vec3 Acceleration()
	{
		Vec3 acceleration = Vec3::Zero();

		detail::DeviceMotion_GetAcceleration(&acceleration.x, &acceleration.y, &acceleration.z);

		return acceleration;
	}

	Vec3 AccelerationIncludingGravity()
	{
		Vec3 acceleration = Vec3::Zero();

		detail::DeviceMotion_GetAccelerationIncludingGravity(&acceleration.x, &acceleration.y, &acceleration.z);

		return acceleration;
	}

	Rotation RotationRate()
	{
		Rotation rotationRate{};

		detail::DeviceMotion_GetAccelerationIncludingGravity(&rotationRate.alpha, &rotationRate.beta, &rotationRate.gamma);

		return rotationRate;
	}

	double Interval()
	{
		return detail::DeviceMotion_GetInterval();
	}
}
