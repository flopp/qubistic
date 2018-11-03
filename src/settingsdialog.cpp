#include <QtWidgets/QFileDialog>
#include "application.h"
#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget* parent) :
    QDialog{parent},
    ui_{new Ui_SettingsDialog}
{
    ui_->setupUi(this);
    
    shapeTypeMapping_
        << qMakePair(ui_->trianglesButton, ShapeType::Triangles)
        << qMakePair(ui_->rectanglesButton, ShapeType::Rectangles)
        << qMakePair(ui_->rectanglesRotatedButton, ShapeType::RectanglesRotated)
        << qMakePair(ui_->circlesButton, ShapeType::Circles)
        << qMakePair(ui_->ellipsesButton, ShapeType::Ellipses)
        << qMakePair(ui_->ellipsesRotatedButton, ShapeType::EllipsesRotated)
        << qMakePair(ui_->polygonsButton, ShapeType::Polygons)
        << qMakePair(ui_->beziersButton, ShapeType::Beziers)
        << qMakePair(ui_->mixedButton, ShapeType::Mixed);
    
    targetTypeMapping_
        << qMakePair(ui_->stepsRadioButton, TargetType::Steps)
        << qMakePair(ui_->scoreRadioButton, TargetType::Score);

    connect(ui_->applyButton, &QAbstractButton::clicked, this, &SettingsDialog::accept);
    connect(ui_->applyRunButton, &QAbstractButton::clicked, this, &SettingsDialog::applyAndRunClicked);
    connect(ui_->cancelButton, &QAbstractButton::clicked, this, &SettingsDialog::reject);
    connect(ui_->browseButton, &QAbstractButton::clicked, this, &SettingsDialog::browseClicked);
    connect(this, &SettingsDialog::accepted, this, &SettingsDialog::saveSettings);

    for (auto p: shapeTypeMapping_) {
        if (p.second == app().settings().shapeType()) {
            p.first->setChecked(true);
        }
    }
    for (auto p: targetTypeMapping_) {
        if (p.second == app().settings().targetType()) {
            p.first->setChecked(true);
        }
    }
    ui_->stepsInput->setValue(app().settings().targetSteps());
    ui_->scoreInput->setValue(app().settings().targetScore());
    ui_->extraShapesInput->setValue(app().settings().extraShapes());
    ui_->pathInput->setText(app().settings().primitiveBinPath());
}

SettingsDialog::~SettingsDialog()
{
    delete ui_;
}

void SettingsDialog::toggleApplyAndRun(bool enabled)
{
    ui_->applyRunButton->setEnabled(enabled);    
}

void SettingsDialog::applyAndRunClicked()
{
    applyAndRun_ = true;
    accept();
}

void SettingsDialog::browseClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select Primitive Executable", ui_->pathInput->text(), "Primitive Executable (primitive);;All Files (*)");
    if (!fileName.isEmpty()) {
        ui_->pathInput->setText(fileName);
    }
}

void SettingsDialog::saveSettings()
{
    for (auto p: shapeTypeMapping_) {
        if (p.first->isChecked()) {
            app().settings().setShapeType(p.second);
        }
    }

    for (auto p: targetTypeMapping_) {
        if (p.first->isChecked()) {
            app().settings().setTargetType(p.second);
        }
    }

    app().settings().setTargetSteps(ui_->stepsInput->value());
    app().settings().setTargetScore(ui_->scoreInput->value());
    app().settings().setExtraShapes(ui_->extraShapesInput->value());
    app().settings().setPrimitiveBinPath(ui_->pathInput->text());
    app().settings().sync();
}
