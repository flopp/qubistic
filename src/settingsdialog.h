#pragma once

#include <QtWidgets/QDialog>
#include "settings.h"
class QAbstractButton;
class Ui_SettingsDialog;

class SettingsDialog: public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    virtual ~SettingsDialog();
    
    void toggleApplyAndRun(bool enabled);

    bool applyAndRun() { return applyAndRun_; }

private slots:
    void applyAndRunClicked();
    void browseClicked();
    void saveSettings();

private:
    Ui_SettingsDialog* ui_{nullptr};
    QList<QPair<QAbstractButton*, ShapeType>> shapeTypeMapping_;
    QList<QPair<QAbstractButton*, TargetType>> targetTypeMapping_;
    bool applyAndRun_{false};
};
