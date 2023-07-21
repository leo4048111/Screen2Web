#define SDL_MAIN_HANDLED

#include "window.h"
#include "httpserver.h"

// Main code
int main(int, char **)
{
    int err = 0;
    err = SCREEN2WEB_NM::HttpServer::GetInstance().Init();
    err = SCREEN2WEB_NM::HttpServer::GetInstance().Listen("0.0.0.0", 13728);

    err = SCREEN2WEB_NM::Window::GetInstance().Init();
    err = SCREEN2WEB_NM::Window::GetInstance().Loop();
    
    err = SCREEN2WEB_NM::HttpServer::GetInstance().CleanUp();
    err = SCREEN2WEB_NM::Window::GetInstance().DeInit();
    return err;
}
