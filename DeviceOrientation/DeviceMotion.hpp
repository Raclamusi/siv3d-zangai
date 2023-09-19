# pragma once

# include <Siv3D/2DShapesFwd.hpp>
# include <Siv3D/Vector3D.hpp>
# include "PermissionStatus.hpp"
# include "Rotation.hpp"

namespace DeviceMotion
{
	/// @brief 可能な場合、端末の動きを取得することに関しての許可を要求します。iOS 13 では許可が必要です。
	void RequestPermission();

	/// @brief 端末の動きを取得することに関しての許可の状態を返します。
	/// @return 許可状態
	[[nodiscard]]
	PermissionStatus Permission();

	/// @brief 端末の加速度を返します。
	/// @remark 端末によっては提供されない可能性があります。その場合は常に Vec3{ 0, 0, 0 } を返します。
	/// @return 端末の加速度 (m/s^2)
	[[nodiscard]]
	s3d::Vec3 Acceleration();

	/// @brief 重力加速度を含む端末の加速度を返します。
	/// @return 重力加速度を含む端末の加速度 (m/s^2)
	[[nodiscard]]
	s3d::Vec3 AccelerationIncludingGravity();

	/// @brief 端末の向きの変化率を返します。
	/// @remark 端末によっては提供されない可能性があります。その場合は常に Rotation{ 0, 0, 0 } を返します。
	/// @return 端末の向きの変化率 (rad/s)
	[[nodiscard]]
	Rotation RotationRate();

	/// @brief 端末からデータを取得する間隔を返します。
	/// @return 端末からデータを取得する間隔 (秒)
	[[nodiscard]]
	double Interval();
}
