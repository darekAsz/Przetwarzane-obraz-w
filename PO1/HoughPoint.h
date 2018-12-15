#pragma once
class CHoughPoint
{
public:
    int i;
    int j;
    int ro;

    CHoughPoint(int ii, int jj, int roo);
    CHoughPoint();
    ~CHoughPoint();

    bool operator < (const CHoughPoint& x) const
    {
        return (ro > x.ro);
    }
};


