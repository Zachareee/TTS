#include "concurrent_queue.h"
#include "player_wrapper.h"

static us_listen_socket_t* sock;
constexpr auto port{8000};

int main()
{
    concurrent_queue<std::string> queue{};

    PlayerWrapper player{queue};

    uWS::App app{};

    app.get("/speak",
            [&](uWS::HttpResponse<false>* res, uWS::HttpRequest* req)
            {
                auto message{req->getQuery("text")};
                spdlog::info("message is: {}", message);

                queue.push(std::string{message});
                res->end("Done");
            })
        .get("/skip",
             [&](uWS::HttpResponse<false>* res, auto...)
             {
                 spdlog::info("message skipped");
                 player.speak("");
                 res->end("Done");
             })
        .listen(port,
                [](us_listen_socket_t* listenSocket)
                {
                    if (listenSocket)
                    {
                        spdlog::info("listening for connections on {}", port);
                        sock = listenSocket;
                        SetConsoleCtrlHandler(
                            [](unsigned long sig) -> int
                            {
                                if (sig == CTRL_C_EVENT)
                                    us_listen_socket_close(0, sock);
                                return true;
                            },
                            true);
                    }
                })
        .run();
}
