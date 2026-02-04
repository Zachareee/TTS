using namespace winrt;

static bool select_voice(
    const Windows::Media::SpeechSynthesis::SpeechSynthesizer& synth)
{
    for (auto voice : synth.AllVoices())
    {
        if (to_string(voice.DisplayName()) == "Microsoft Ayumi")
        {
            synth.Voice(voice);
            return true;
        }
    }
    return false;
}

int main()
{
    Windows::Media::SpeechSynthesis::SpeechSynthesizer synth{};
    if (!select_voice(synth))
    {
        spdlog::error("Ayumi not found");
        return 1;
    }

    hstring text{to_hstring("Hello world")};
    auto stream{synth.SynthesizeTextToStreamAsync(text).get()};

    Windows::Media::Playback::MediaPlayer player{};

    player.SetStreamSource(stream);
    player.Play();

    std::promise<void> p;
    std::future<void> future = p.get_future();

    player.MediaEnded(
        [&](auto...)
        {
            player.Close();
            p.set_value();
        });
    future.get();
}
