#ifndef BLACKOUTCONF_H
#define BLACKOUTCONF_H

/**
    @author Erik Kaju
*/
class BlackoutConf{
public:
    BlackoutConf();

    ~BlackoutConf();
    int throwerXFrom;
    int throwerXTo;
    int throwerY;
    int cornerLeftX;
    int cornerLeftY;
    int cornerRightX;
    int cornerRightY;

    int getThrowerXTo()
    {
        return throwerXTo;
    }

    void setThrowerXTo(int value)
    {
        throwerXTo = value;
    }

    int getThrowerY()
    {
        return throwerY;
    }

    void setThrowerY(int value)
    {
        throwerY = value;
    }

    int getCornerLeftX()
    {
        return cornerLeftX;
    }

    void setCornerLeftX(int value)
    {
        cornerLeftX = value;
    }

    int getCornerLeftY()
    {
        return cornerLeftY;
    }

    void setCornerLeftY(int value)
    {
        cornerLeftY = value;
    }

    int getCornerRightX()
    {
        return cornerRightX;
    }

    void setCornerRightX(int value)
    {
        cornerRightX = value;
    }

    int getCornerRightY()
    {
        return cornerRightY;
    }

    void setCornerRightY(int value)
    {
        cornerRightY = value;
    }

    int getThrowerXFrom()
    {
        return throwerXFrom;
    }

    void setThrowerXFrom(int value)
    {
        throwerXFrom = value;
    }

};
#endif // BLACKOUTCONF_H
