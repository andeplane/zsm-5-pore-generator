#include "nogui.h"
#include "zsm5geometry.h"
#include "montecarlo.h"
#include "statistics/statistics.h"
NoGUI::NoGUI()
{
    geometry = new Zsm5geometry();
    monteCarlo = new MonteCarlo();

}
