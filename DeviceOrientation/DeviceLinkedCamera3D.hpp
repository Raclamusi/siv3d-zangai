# pragma once

# include <Siv3D/BasicCamera3D.hpp>
# include <Siv3D/Scene.hpp>
# include <Siv3D/Vector2D.hpp>
# include <Siv3D/Vector3D.hpp>
# include <Siv3D/Rect.hpp>
# include "Rotation.hpp"

class DeviceLinkedCamera3D : public s3d::BasicCamera3D
{
public:

	DeviceLinkedCamera3D() = default;

	DeviceLinkedCamera3D(const DeviceLinkedCamera3D&) = default;

	explicit DeviceLinkedCamera3D(double verticalFOV = DefaultVerticalFOV, const s3d::Vec3& eyePosition = s3d::Vec3{ 0, 4, -4 }, const Rotation& offsetRotation = Rotation{ 0, 0, 0 }, double nearClip = DefaultNearClip) noexcept;

	const Rotation& getOffset() const noexcept;

	void setOffset(const Rotation& offset) noexcept;

	void updateOrientation();

	void updateTouchUI(double scale = 1.0, double speed = 1.0, const s3d::Rect& region = s3d::Scene::Rect());

	void drawTouchUI(double scale = 1.0);

private:

	Rotation m_offset{};

	bool m_touched = false;

	s3d::Vec2 m_touchUICenter{};

	s3d::Vec2 m_stickPos{};
};
