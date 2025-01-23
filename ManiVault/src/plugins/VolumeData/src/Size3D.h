#pragma once

#include <QDebug>

class Size3D
{
public:
    Size3D();
    Size3D(int width, int height, int depth);

    bool isEmpty() const;
    bool isNull() const;

    int width() const;
    int height() const;
    int depth() const;

    void setWidth(int width);
    void setHeight(int height);
    void setDepth(int depth);

    bool operator==(const Size3D &other) const;
    bool operator!=(const Size3D &other) const;

private:
    int _width;
    int _height;
    int _depth;
};

QDebug operator<<(QDebug dbg, const Size3D &size);
