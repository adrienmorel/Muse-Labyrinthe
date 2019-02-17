#ifndef AMAZED_MUSE_H
#define AMAZED_MUSE_H

#endif //AMAZED_MUSE_H

#include <iostream>
#include <windows.h>

using namespace std;

class Muse {
private:
    string domain = "muse-labyrinthe.herokuapp.com";
    static LPTHREAD_START_ROUTINE getRoutine;
public:
    Muse();
    DWORD WINAPI get(LPVOID lpProgress);
};

