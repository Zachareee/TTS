#include "queue.h"

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
    Windows::Media::SpeechSynthesis::SpeechSynthesizer m_synth{};

  public:
    PlayerWrapper(concurrent_queue<std::string>* q)
    {
        winrt::init_apartment();

        if (!select_voice(m_synth))
        {
            spdlog::error("Ayumi not found");
        }

        m_player.AutoPlay(true);

        m_player.MediaEnded(
            [&](auto...)
            {
                std::string msg;
                q->wait_and_pop(msg);
                this->speak(msg);
            });

        this->speak("");
    }
    ~PlayerWrapper()
    {
        spdlog::error("PlayerWrapper dropped");
        m_player.Close();
    }

    void speak(auto message)
    {
        m_player.SetStreamSource(m_synth.SynthesizeTextToStreamAsync(to_hstring(message)).get());
    }
};

[[noreturn]] static void thread_job(concurrent_queue<std::string>* q)
{
    PlayerWrapper player{q};
    while (true)
    {
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main()
{
    concurrent_queue<std::string> queue{};

    std::thread player_thread{thread_job, &queue};

    uWS::App app{};

    app.get("/speak",
            [&](uWS::HttpResponse<false>* res, uWS::HttpRequest* req)
            {
                auto message{req->getQuery("text")};
                spdlog::info("message is: {}", message);

                queue.push(std::string{message});
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
