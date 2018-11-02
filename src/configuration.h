#pragma once

#include <QtCore/QString>

class Configuration
{
public:    
    enum class ShapeType
    {
        Triangles = 1,
        Rectangles = 2,
        RectanglesRotated = 5,
        Circles = 4,
        Ellipses = 3,
        EllipsesRotated = 7,
        Polygons = 8,
        Beziers = 6,
        Mixed = 0
    };
    ShapeType shapeType{ShapeType::Triangles};
    
    enum class TargetType
    {
        Shapes,
        Score
    };
    TargetType targetType{TargetType::Shapes};
    int targetShapes{100};
    double targetScore{95.9};

    QString primitivePath{"primitive"};
};
