# pragma once

# include <Siv3D/Types.hpp>

/// @brief �����̌���
enum class ScreenOrientationType : s3d::uint8
{
	/// @brief 1�ڂ̃|�[�g���[�g�i�����j���[�h
	PortraitPrimary,

	/// @brief 2�ڂ̃|�[�g���[�g�i�����j���[�h
	PortraitSecondary,

	/// @brief 1�ڂ̃����h�X�P�[�v�i�c���j���[�h
	LandscapePrimary,

	/// @brief 2�ڂ̃����h�X�P�[�v�i�c���j���[�h
	LandscapeSecondary,
};
