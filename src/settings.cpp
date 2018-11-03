#include "settings.h"

Settings::Settings()
{
    shapeTypeMapping_
        << qMakePair(ShapeType::Triangles, QString("Triangles"))
        << qMakePair(ShapeType::Rectangles, QString("Rectangles"))
        << qMakePair(ShapeType::RectanglesRotated, QString("RectanglesRotated"))
        << qMakePair(ShapeType::Circles, QString("Circles"))
        << qMakePair(ShapeType::Ellipses, QString("Ellipses"))
        << qMakePair(ShapeType::EllipsesRotated, QString("EllipsesRotated"))
        << qMakePair(ShapeType::Polygons, QString("Polygons"))
        << qMakePair(ShapeType::Beziers, QString("Beziers"))
        << qMakePair(ShapeType::Mixed, QString("Mixed"));
    targetTypeMapping_
        << qMakePair(TargetType::Steps, QString("Steps"))
        << qMakePair(TargetType::Score, QString("Score"));

    QString shape = settings_.value("shapeType").value<QString>();
    for (const auto p: shapeTypeMapping_) {
        if (p.second == shape) {
            shapeType_ = p.first;
        }
    }

    QString target = settings_.value("targetType").value<QString>();
    for (const auto p: targetTypeMapping_) {
        if (p.second == target) {
            targetType_ = p.first;
        }
    }

    targetSteps_ = settings_.value("targetShapes").value<int>();
    if (targetSteps_ <= 0) {
        targetSteps_ = 100;
    }
    targetScore_ = settings_.value("targetScore").value<double>();
    if ((targetScore_ < 0.0) || (targetScore_ > 100.0)) {
        targetScore_ = 95.0;
    }
    extraShapes_ = settings_.value("extraShapes").value<int>();
    if (extraShapes_ < 0) {
        extraShapes_ = 0;
    } else if (extraShapes_ > 10) {
        extraShapes_ = 10;
    }

    primitiveBinPath_ = settings_.value("primitiveBinPath").value<QString>();
    if (primitiveBinPath_.isEmpty()) {
        primitiveBinPath_ = "primitive";
    }
}

void Settings::sync()
{
    for (const auto p: shapeTypeMapping_) {
        if (p.first == shapeType_) {
            settings_.setValue("shapeType", p.second);
            break;
        }
    }
    for (const auto p: targetTypeMapping_) {
        if (p.first == targetType_) {
            settings_.setValue("targetType", p.second);
            break;
        }
    }
    settings_.setValue("targetShapes", targetSteps_);
    settings_.setValue("targetScore", targetScore_);
    settings_.setValue("extraShapes", extraShapes_);
    settings_.setValue("primitiveBinPath", primitiveBinPath_);
    settings_.sync();
}
