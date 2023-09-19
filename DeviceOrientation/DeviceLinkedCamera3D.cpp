# include "DeviceLinkedCamera3D.hpp"

# include <Siv3D.hpp>
# include "DeviceOrientation.hpp"

DeviceLinkedCamera3D::DeviceLinkedCamera3D(double verticalFOV, const Vec3& eyePosition, const Rotation& offsetRotation, double nearClip) noexcept
	: BasicCamera3D(Scene::Size(), verticalFOV, eyePosition, Vec3{ 0, 0, 0 }, Vec3{ 0, 1, 0 }, nearClip)
	, m_offset{ offsetRotation }
{
}

const Rotation& DeviceLinkedCamera3D::getOffset() const noexcept
{
	return m_offset;
}

void DeviceLinkedCamera3D::setOffset(const Rotation& offset) noexcept
{
	m_offset = offset;
}

void DeviceLinkedCamera3D::updateOrientation()
{
	auto [alpha, beta, gamma] = DeviceOrientation::Orientation();

	alpha += m_offset.alpha;
	beta += m_offset.beta;
	gamma += m_offset.gamma;

	const auto orientation = Quaternion::RollPitchYaw(-beta, -alpha, -gamma);
	const Vec3 focusVector = (orientation * Float3{ 0, -1, 0 });
	const Vec3 upDirection = (orientation * Float3{ 0, 0, 1 });

	setSceneSize(Graphics3D::GetRenderTargetSize());

	setView(m_eyePosition, (m_eyePosition + focusVector), upDirection);
}

void DeviceLinkedCamera3D::updateTouchUI(double scale, double speed, const Rect& region)
{
	if (region.leftClicked())
	{
		m_touched = true;
		m_touchUICenter = Cursor::PosF();
		m_stickPos = Cursor::PosF();
	}
	else if (m_touched)
	{
		if (MouseL.up())
		{
			m_touched = false;
		}
		else if (MouseL.pressed())
		{
			const double scaledSpeed = (speed * 4 * Scene::DeltaTime());
			const double lengthLimit = (150 * scale);
			const Vec2 stickDelta = (Cursor::PosF() - m_touchUICenter).limitLength(lengthLimit);

			m_stickPos = (m_touchUICenter + stickDelta);

			if (not stickDelta.isZero())
			{
				const Vec3 focusVector = (m_focusPosition - m_eyePosition);
				const double eyeDeltaLength = (stickDelta.length() / lengthLimit * scaledSpeed);
				const double stickAngle = Vec2{ 0, -1 }.getAngle(stickDelta);
				const Vec3 eyeDelta = (focusVector.withLength(eyeDeltaLength) * Quaternion::RotationAxis(m_upDirection, stickAngle));

				setView((m_eyePosition + eyeDelta), (m_focusPosition + eyeDelta), m_upDirection);
			}
		}
	}
}

void DeviceLinkedCamera3D::drawTouchUI(double scale)
{
	if (m_touched)
	{
		Circle{ m_touchUICenter, (150 * scale) }.draw(ColorF{ 1, 0.3 });

		Circle{ m_stickPos, (100 * scale) }.draw(ColorF{ 1, 0.8 });
	}
}
