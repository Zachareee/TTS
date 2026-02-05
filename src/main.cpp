#include <spdlog/spdlog.h>
#include <string_view>
using namespace winrt;

static bool
select_voice(const Windows::Media::SpeechSynthesis::SpeechSynthesizer& synth)
{
    for (auto voice : synth.AllVoices())
    {
        if (to_string(voice.DisplayName()).find("Microsoft Ayumi") !=
            std::string::npos)
        {
            synth.Voice(voice);
            return true;
        }
    }
    return false;
}

class PlayerWrapper final
{
  private:
    Windows::Media::Playback::MediaPlayer m_player{};

  public:
    PlayerWrapper(auto lambda)
    {
        m_player.AutoPlay(true);
        m_player.MediaEnded(lambda);
    }
    ~PlayerWrapper() { m_player.Close(); }

    void speak(const Windows::Media::SpeechSynthesis::SpeechSynthesizer& synth,
               auto message)
    {
        m_player.SetStreamSource(
            synth.SynthesizeTextToStreamAsync(to_hstring(message)).get());
    }
};

int main()
{
    Windows::Media::SpeechSynthesis::SpeechSynthesizer synth{};
    if (!select_voice(synth))
    {
        spdlog::error("Ayumi not found");
        return 1;
    }

    std::promise<void> p{};
    std::future<void> future{p.get_future()};
    p.set_value();

    PlayerWrapper player{[&](auto...) { p.set_value(); }};

    uWS::App app{};

    app.get("/speak",
            [&](uWS::HttpResponse<false>* res, uWS::HttpRequest* req)
            {
                auto message{req->getQuery("text")};
                spdlog::info("message is: {}", message);
                future.get();

                p = {};
                future = p.get_future();
                player.speak(synth, message);
                res->end("Done");
            })
        .listen(8000,
                [](auto* listenSocket)
                {
                    if (listenSocket)
                        spdlog::info("Listening for connections");
                })
        .run();
}
