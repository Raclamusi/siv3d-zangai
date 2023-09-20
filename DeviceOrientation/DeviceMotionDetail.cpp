# include <emscripten.h>

namespace DeviceMotion::detail
{
	[[maybe_unused]]
	static int g_dummy = []
	{
		EM_ASM
		(({
			window.siv3dDeviceMotion = {
				permission: (DeviceMotionEvent.requestPermission ? "default" : "granted"),
				is_iOS: (/iPad|iPhone|iPod/.test(navigator.userAgent) || (navigator.platform === "MacIntel" && navigator.maxTouchPoints > 1)),
				event: {},
				setEvent: function () {
					window.addEventListener("devicemotion", event => this.event = event);
				},
			};
			window.siv3dDeviceOrientation = {
				permission: (DeviceOrientationEvent.requestPermission ? "default" : "granted"),
				event: {},
				setEvent: function () {
					window.addEventListener("deviceorientation", event => this.event = event);
				},
			};

			if (DeviceMotionEvent.requestPermission) {
				const button = document.createElement("button");
				button.style.position = "absolute";
				button.style.left = "5%";
				button.style.top = "calc(50% - 1em)";
				button.style.width = "90%";
				button.style.height = "2em";
				button.style.fontSize = "2rem";
				button.style.fontWeight = "bold";
				button.textContent = "Get Permissions";
				const request = async () => {
					const permission = await DeviceMotionEvent.requestPermission();
					if (permission === "granted" && siv3dDeviceMotion.permission !== "granted") {
						siv3dDeviceMotion.setEvent();
						siv3dDeviceOrientation.setEvent();
						button.remove();
					}
					siv3dDeviceMotion.permission = permission;
					siv3dDeviceOrientation.permission = permission;
				};
				button.addEventListener("click", request);
				document.getElementById("app").appendChild(button);
			}
			else {
				siv3dDeviceMotion.setEvent();
				siv3dDeviceOrientation.setEvent();
			}
		}));

		return 0;
	}();
}
