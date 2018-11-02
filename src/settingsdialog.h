#pragma once

#include <QtWidgets/QDialog>
#include "configuration.h"
class QAbstractButton;
class Ui_SettingsDialog;

class SettingsDialog: public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    virtual ~SettingsDialog();
    
    void toggleApplyAndRun(bool enabled);
    Configuration config() const;
    void setConfig(const Configuration& config);
    
    bool applyAndRun() { return applyAndRun_; }

private slots:
    void applyAndRunClicked();
    void browseClicked();
    
private:
    Ui_SettingsDialog* ui_{nullptr};
    QList<QPair<QAbstractButton*, Configuration::ShapeType>> shapeTypeMapping_;
    QList<QPair<QAbstractButton*, Configuration::TargetType>> targetTypeMapping_;
    bool applyAndRun_{false};
};
