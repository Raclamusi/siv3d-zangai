# pragma once

# include "PermissionStatus.hpp"
# include "Rotation.hpp"

namespace DeviceOrientation
{
	/// @brief 端末の向きを取得することに関しての許可の状態を返します。
	/// @return 許可状態
	[[nodiscard]]
	PermissionStatus Permission();

	/// @brief 端末の物理的な向きを返します。
	/// @remark 0 ≤ alpha < 2π
	/// @remark －π ≤ beta < π
	/// @remark －π/2 ≤ gamma < π/2
	/// @return 端末の向き
	[[nodiscard]]
	Rotation Orientation();

	/// @brief 端末の向きが絶対的に（地球座標フレームを基準として）提供されているかを返します。
	/// @return 地球座標フレームを基準としている場合 true, それ以外の場合は false
	[[nodiscard]]
	bool IsAbsolute();
}
