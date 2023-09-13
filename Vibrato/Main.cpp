# include <Siv3D.hpp> // OpenSiv3D v0.6.11

void Main()
{
	Audio audio{ U"example/test.mp3", Loop::Yes };

	Microphone mic{ unspecified };

	Wave recordingBuffer(0, mic.getSampleRate());

	bool vibrato = false;

	double vibratoAmplitude = 1.5;

	double vibratoFrequency = 3.0;

	if (not GlobalAudio::SupportsPitchShift())
	{
		Print << U"ピッチシフトフィルタがサポートされていません！";
	}

	while (System::Update())
	{
		if (mic.isRecording())
		{
			auto&& buffer = mic.getBuffer();

			const auto first = (buffer.begin() + recordingBuffer.size() % mic.getBufferLength());

			const auto last = (buffer.begin() + mic.posSample() + 1);

			if (last < first)
			{
				recordingBuffer.insert(recordingBuffer.end(), first, buffer.end());
				recordingBuffer.insert(recordingBuffer.end(), buffer.begin(), last);
			}
			else
			{
				recordingBuffer.insert(recordingBuffer.end(), first, last);
			}
		}

		if (SimpleGUI::Button(U"\U000f0770 Open", Vec2{ 50, 40 }, 120, (not mic.isRecording())))
		{
			if (auto newAudio = Dialog::OpenAudio())
			{
				audio = std::move(newAudio);

				audio.setLoop(true);

				audio.play();
			}
		}

		if (SimpleGUI::Button((mic.isRecording() ? U"\U000f04Db Stop recording" : U"\U000f036c Record"), Vec2{ 180, 40 }, 200, mic.isOpen()))
		{
			if (mic.isRecording())
			{
				mic.stop();

				audio = Audio{ recordingBuffer, Loop::Yes };

				audio.play();

				recordingBuffer.clear();
			}
			else
			{
				audio.pause();

				mic.start();
			}
		}

		{
			Rect{ 390, 40, 360, 36 }.draw(Palette::White);

			RoundRect{ 478, 55, 264, 6, 2 }.draw(ColorF{ 0.33 });

			RoundRect{ 478, 55, 264 * mic.mean(), 6, 2 }.draw(ColorF{ 0.35, 0.7, 1.0 });

			SimpleGUI::GetFont()(FormatTime(SecondsF{ recordingBuffer.lengthSec() }, U"M:ss")).draw(Arg::leftCenter(398, 57), Palette::Black);
		}

		if (SimpleGUI::Button((audio.isPlaying() ? U"\U000f03e4" : U"\U000f040a"), Vec2{ 50, 120 }, 60, (not mic.isRecording())))
		{
			if (audio.isPlaying())
			{
				audio.pause();
			}
			else
			{
				audio.play();
			}
		}

		const String time = U"{} / {}"_fmt(FormatTime(SecondsF{ audio.posSec() }, U"M:ss"), FormatTime(SecondsF{ audio.lengthSec() }, U"M:ss"));
		double progress = (static_cast<double>(audio.posSample()) / audio.samples());
		if (SimpleGUI::Slider(time, progress, Vec2{ 120, 120 }, 130, 500))
		{
			audio.pause();

			audio.seekSamples(static_cast<size_t>(audio.samples() * progress));
		}

		if (SimpleGUI::CheckBox(vibrato, U"Vibrato", Vec2{ 50, 200 }, 150, GlobalAudio::SupportsPitchShift()))
		{
			if (not vibrato)
			{
				GlobalAudio::BusSetPitchShiftFilter(MixBus0, 0, 0);
			}
		}

		SimpleGUI::Slider(U"Amplitude: {:.2f}"_fmt(vibratoAmplitude), vibratoAmplitude, 0, 12, Vec2{ 210, 200 }, 180, 360);

		SimpleGUI::Slider(U"Frequency: {:.2f}"_fmt(vibratoFrequency), vibratoFrequency, 0, 24, Vec2{ 210, 240 }, 180, 360);

		if (vibrato)
		{
			const double pitchShift = vibratoAmplitude * Sin(2_pi * vibratoFrequency * Scene::Time());

			GlobalAudio::BusSetPitchShiftFilter(MixBus0, 0, pitchShift);
		}
	}
}
