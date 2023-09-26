# include <Siv3D.hpp> // OpenSiv3D for Web v0.6.6r1
# include "DeviceLinkedCamera3D.hpp"
# include "DeviceOrientation.hpp"
# include "DeviceMotion.hpp"

// 風車の回転する羽根を描く関数
void DrawMillModel(const Model& model, const Mat4x4& mat)
{
	const auto& materials = model.materials();

	for (const auto& object : model.objects())
	{
		Mat4x4 m = Mat4x4::Identity();

		// 風車の羽根の回転
		if (object.name == U"Mill_Blades_Cube.007")
		{
			m *= Mat4x4::Rotate(Vec3{ 0,0,-1 }, (Scene::Time() * -120_deg), Vec3{ 0, 9.37401, 0 });
		}

		const Transformer3D transform{ (m * mat) };

		object.draw(materials);
	}
}

void Main()
{
	// リサイズ可能にする
	Window::SetStyle(WindowStyle::Sizable);
	Scene::SetResizeMode(ResizeMode::Actual);

	// フォント
	const Font font{ FontMethod::MSDF, 48 };

	// 背景色
	const ColorF backgroundColor = ColorF{ 0.4, 0.6, 0.8 }.removeSRGBCurve();

	// 地面
	const Mesh groundPlane{ MeshData::OneSidedPlane(2000, { 400, 400 }) };
	const Texture groundTexture{ U"example/texture/ground.jpg", TextureDesc::MippedSRGB };

	// 地球
	constexpr Sphere earthSphere{ { 0, 1, 0 }, 1 };
	const Texture earthTexture{ U"example/texture/earth.jpg", TextureDesc::MippedSRGB };

	// モデルデータをロード
	const Model blacksmithModel{ U"example/obj/blacksmith.obj" };
	const Model millModel{ U"example/obj/mill.obj" };
	const Model treeModel{ U"example/obj/tree.obj" };
	const Model pineModel{ U"example/obj/pine.obj" };
	const Model siv3dkunModel{ U"example/obj/siv3d-kun.obj" };

	// モデルに付随するテクスチャをアセット管理に登録
	Model::RegisterDiffuseTextures(treeModel, TextureDesc::MippedSRGB);
	Model::RegisterDiffuseTextures(pineModel, TextureDesc::MippedSRGB);
	Model::RegisterDiffuseTextures(siv3dkunModel, TextureDesc::MippedSRGB);

	// レンダ―テクスチャ
	const MSRenderTexture renderTexture{ 2000, 2000, TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };

	// カメラ
	DeviceLinkedCamera3D camera{ 40_deg, Vec3{ 0, 3, -16 } };
	bool longPress = false;
	
	while (System::Update())
	{
		// [カメラの更新]
		{
			// 長押しで原点の方角を向く
			if (MouseL.down())
			{
				longPress = true;
			}
			else if (MouseL.pressed())
			{
				if (not Cursor::Delta().isZero())
				{
					longPress = false;
				}
				else if (longPress && (MouseL.pressedDuration() >= 0.8s))
				{
					longPress = false;

					if (const auto eyePos = camera.getEyePosition().xz(); not eyePos.isZero())
					{
						const auto focusPos = camera.getFocusPosition().xz();
						const auto focusVec = ((focusPos == eyePos) ? camera.getUpDirection().xz() : (focusPos - eyePos));
						const double offsetAlpha = (camera.getOffset().alpha + focusVec.getAngle(-eyePos));

						camera.setOffset(Rotation{ offsetAlpha, 0, 0 });
					}
				}
			}

			camera.updateOrientation();
			camera.updateTouchUI();
			Graphics3D::SetCameraTransform(camera);
		}

		// [3D シーンの描画]
		{
			const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };

			// [モデルの描画]
			{
				// 地面の描画
				groundPlane.draw(groundTexture);

				// 地球の描画
				earthSphere.draw(earthTexture);

				// 鍛冶屋の描画
				blacksmithModel.draw(Vec3{ 8, 0, 4 });

				// 風車の描画
				DrawMillModel(millModel, Mat4x4::Translate(-8, 0, 4));

				// 木の描画
				{
					const ScopedRenderStates3D renderStates{ BlendState::OpaqueAlphaToCoverage, RasterizerState::SolidCullNone };
					treeModel.draw(Vec3{ 16, 0, 4 });
					pineModel.draw(Vec3{ 16, 0, 0 });
				}

				// Siv3D くんの描画
				siv3dkunModel.draw(Vec3{ 2, 0, -2 }, Quaternion::RotateY(180_deg));
			}
		}

		// [RenderTexture を 2D シーンに描画]
		{
			Graphics3D::Flush();
			renderTexture.resolve();
			Shader::LinearToScreen(renderTexture);
		}

		// [加速度の描画]
		{
			Rect{ 0, 0, 500, 300 }.draw(ColorF{ 0, 0.8 });

			auto [x, y, z] = DeviceMotion::Acceleration();

			font(U"acceleration.x: {:6.2f}"_fmt(x)).draw(30, 20, 20, Palette::White);
			font(U"acceleration.y: {:6.2f}"_fmt(y)).draw(30, 20, 110, Palette::White);
			font(U"acceleration.z: {:6.2f}"_fmt(z)).draw(30, 20, 200, Palette::White);

			Rect{ 50, 70, 400, 20 }.draw(Palette::Lightgray);
			RectF{ 250, 70, Clamp(x * 10, -200.0, 200.0), 20 }.draw(Palette::Orange);

			Rect{ 50, 160, 400, 20 }.draw(Palette::Lightgray);
			RectF{ 250, 160, Clamp(y * 10, -200.0, 200.0), 20 }.draw(Palette::Orange);

			Rect{ 50, 250, 400, 20 }.draw(Palette::Lightgray);
			RectF{ 250, 250, Clamp(z * 10, -200.0, 200.0), 20 }.draw(Palette::Orange);
		}

		// [カメラ操作 UI の描画]
		{
			camera.drawTouchUI();
		}
	}
}
