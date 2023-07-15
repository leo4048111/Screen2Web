#include "window.h"

#undef main

// Main code
int main(int, char **)
{
    int err = 0;
    err = SCREEN2WEB_NM::Window::GetInstance().Init();
    err = SCREEN2WEB_NM::Window::GetInstance().Loop();
    err = SCREEN2WEB_NM::Window::GetInstance().DeInit();
    return err;
}
