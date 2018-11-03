#pragma once

#include <QtCore/QSettings>

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

enum class TargetType
{
    Shapes,
    Score
};

class Settings
{
    public:
        Settings();

        void sync();

        const ShapeType& shapeType() const { return shapeType_; }
        void setShapeType(const ShapeType& shapeType) { shapeType_ = shapeType; }

        const TargetType& targetType() const { return targetType_; }
        void setTargetType(const TargetType& targetType) { targetType_ = targetType; }

        int targetShapes() const { return targetShapes_; }
        void setTargetShapes(int targetShapes) { targetShapes_ = targetShapes; }

        double targetScore() const { return targetScore_; }
        void setTargetScore(double targetScore) { targetScore_ = targetScore; }

        const QString& primitiveBinPath() const { return primitiveBinPath_; }
        void setPrimitiveBinPath(const QString& primitiveBinPath) { primitiveBinPath_ = primitiveBinPath; }

    private:
        QSettings settings_;

        ShapeType shapeType_{ShapeType::Triangles};
        TargetType targetType_{TargetType::Shapes};
        int targetShapes_{100};
        double targetScore_{95.0};
        QString primitiveBinPath_{"primitive"};

        QList<QPair<ShapeType, QString>> shapeTypeMapping_;
        QList<QPair<TargetType, QString>> targetTypeMapping_;
};
