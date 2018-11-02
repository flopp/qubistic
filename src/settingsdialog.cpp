#include <QtWidgets/QFileDialog>
#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget* parent) :
    QDialog{parent},
    ui_{new Ui_SettingsDialog}
{
    ui_->setupUi(this);
    
    shapeTypeMapping_
        << qMakePair(ui_->trianglesButton, Configuration::ShapeType::Triangles)
        << qMakePair(ui_->rectanglesButton, Configuration::ShapeType::Rectangles)
        << qMakePair(ui_->rectanglesRotatedButton, Configuration::ShapeType::RectanglesRotated)
        << qMakePair(ui_->circlesButton, Configuration::ShapeType::Circles)
        << qMakePair(ui_->ellipsesButton, Configuration::ShapeType::Ellipses)
        << qMakePair(ui_->ellipsesRotatedButton, Configuration::ShapeType::EllipsesRotated)
        << qMakePair(ui_->polygonsButton, Configuration::ShapeType::Polygons)
        << qMakePair(ui_->beziersButton, Configuration::ShapeType::Beziers)
        << qMakePair(ui_->mixedButton, Configuration::ShapeType::Mixed);
    
    targetTypeMapping_
        << qMakePair(ui_->shapesRadioButton, Configuration::TargetType::Shapes)
        << qMakePair(ui_->scoreRadioButton, Configuration::TargetType::Score);

    connect(ui_->applyButton, &QAbstractButton::clicked, this, &SettingsDialog::accept);
    connect(ui_->applyRunButton, &QAbstractButton::clicked, this, &SettingsDialog::applyAndRunClicked);
    connect(ui_->cancelButton, &QAbstractButton::clicked, this, &SettingsDialog::reject);
    connect(ui_->browseButton, &QAbstractButton::clicked, this, &SettingsDialog::browseClicked);
}

SettingsDialog::~SettingsDialog()
{
    delete ui_;
}

void SettingsDialog::toggleApplyAndRun(bool enabled)
{
    ui_->applyRunButton->setEnabled(enabled);    
}

Configuration SettingsDialog::config() const
{
    Configuration config;

    for (auto p: shapeTypeMapping_)
    {
        if (p.first->isChecked())
        {
            config.shapeType = p.second;
        }
    }

    for (auto p: targetTypeMapping_)
    {
        if (p.first->isChecked())
        {
            config.targetType = p.second;
        }
    }

    config.targetShapes = ui_->shapesInput->value();
    config.targetScore = ui_->scoreInput->value();

    config.primitivePath = ui_->pathInput->text();
    return config;
}

void SettingsDialog::setConfig(const Configuration& config)
{
    for (auto p: shapeTypeMapping_)
    {
        if (p.second == config.shapeType)
        {
            p.first->setChecked(true);
        }
    }
    for (auto p: targetTypeMapping_)
    {
        if (p.second == config.targetType)
        {
            p.first->setChecked(true);
        }
    }
    ui_->shapesInput->setValue(config.targetShapes);
    ui_->scoreInput->setValue(config.targetScore);
    ui_->pathInput->setText(config.primitivePath);
}

void SettingsDialog::applyAndRunClicked()
{
    applyAndRun_ = true;
    accept();
}

void SettingsDialog::browseClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select Primitive Executable", ui_->pathInput->text(), "Primitive Executable (primitive);;All Files (*)");
    if (!fileName.isEmpty())
    {
        ui_->pathInput->setText(fileName);
    }
}
