# pragma once

/// @brief 向きを表すクラス
/// @remark https://developer.mozilla.org/ja/docs/Web/API/Device_orientation_events/Orientation_and_motion_data_explained
struct Rotation
{
	/// @brief z 軸を軸とする右ねじ方向の回転角
	double alpha;

	/// @brief x 軸を軸とする右ねじ方向の回転角
	double beta;

	/// @brief y 軸を軸とする右ねじ方向の回転角
	double gamma;
};
