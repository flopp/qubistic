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
    Steps,
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

        int targetSteps() const { return targetSteps_; }
        void setTargetSteps(int targetSteps) { targetSteps_ = targetSteps; }

        double targetScore() const { return targetScore_; }
        void setTargetScore(double targetScore) { targetScore_ = targetScore; }

        const QString& primitiveBinPath() const { return primitiveBinPath_; }
        void setPrimitiveBinPath(const QString& primitiveBinPath) { primitiveBinPath_ = primitiveBinPath; }

        int extraShapes() const { return extraShapes_; }
        void setExtraShapes(int extraShapes) { extraShapes_ = extraShapes; }
    private:
        QSettings settings_;

        ShapeType shapeType_{ShapeType::Triangles};
        TargetType targetType_{TargetType::Steps};
        int targetSteps_{100};
        double targetScore_{95.0};
        QString primitiveBinPath_{"primitive"};
        int extraShapes_{0};

        QList<QPair<ShapeType, QString>> shapeTypeMapping_;
        QList<QPair<TargetType, QString>> targetTypeMapping_;
};
