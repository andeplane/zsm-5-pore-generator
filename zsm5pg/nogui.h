#ifndef NOGUI_H
#define NOGUI_H
#include "inifile.h"

class NoGUI
{
public:
    int steps = 0;
    int printEvery = 100;
    class Zsm5geometry *geometry = nullptr;
    class Statistic *statistic = nullptr;
    class MonteCarlo* monteCarlo = nullptr;
    NoGUI();
    void loadIniFile(IniFile &iniFile);
    void run();
};

#endif // NOGUI_H
