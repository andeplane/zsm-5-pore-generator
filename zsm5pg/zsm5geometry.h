#ifndef ZSM5GEOMETRY_H
#define ZSM5GEOMETRY_H
#include <vector>
using std::vector;
class Zsm5geometry
{
private:
    vector<float> m_planePositionsX;
    vector<float> m_planePositionsY;
    vector<float> m_planePositionsZ;
    int m_planesPerDimension = 10;
    double m_planeSize = 10;
    float m_volume = 0;

public:
    Zsm5geometry();
    ~Zsm5geometry();
    vector<float> &planePositionsX() { return m_planePositionsX; }
    vector<float> &planePositionsY() { return m_planePositionsY; }
    vector<float> &planePositionsZ() { return m_planePositionsZ; }
    int planesPerDimension() const;
    void setPlanesPerDimension(int planesPerDimension);
    void reset();
    void followGradient(Zsm5geometry &gradient, float eps);
    double planeSize() const;
    void setPlaneSize(double planeSize);
    void computeVolume();
};

#endif // ZSM5GEOMETRY_H
