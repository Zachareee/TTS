#pragma once

#include "concurrent_queue.h"

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
    PlayerWrapper(concurrent_queue<std::string>& q)
    {
        if (!select_voice(m_synth))
        {
            spdlog::error("Ayumi not found");
        }

        m_player.AutoPlay(true);

        m_player.MediaEnded(
            [&](auto...)
            {
                std::string msg{q.wait_and_pop()};
                this->speak(msg);
            });

        this->speak("");
    }

    ~PlayerWrapper()
    {
        spdlog::info("PlayerWrapper dropped");
        m_player.Close();
    }

    void speak(auto message)
    {
        m_player.SetStreamSource(
            m_synth.SynthesizeTextToStreamAsync(to_hstring(message)).get());
    }
};
