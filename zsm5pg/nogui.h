#ifndef NOGUI_H
#define NOGUI_H

class NoGUI
{
public:
    class Zsm5geometry *geometry = nullptr;
    class Statistic *statistic = nullptr;
    class MonteCarlo* monteCarlo = nullptr;
    NoGUI();
};

#endif // NOGUI_H
