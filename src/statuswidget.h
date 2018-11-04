#pragma once

#include <QtWidgets/QWidget>
class Ui_StatusWidget;

class StatusWidget: public QWidget
{
    Q_OBJECT

public:
    explicit StatusWidget(QWidget* parent = nullptr, QString label = QString{"Status"});
    virtual ~StatusWidget();

    void setLabel(const QString& label);

    void clearValue();
    void setValue(int value);
    void setValue(double value);

private:
    Ui_StatusWidget* ui_{nullptr};
};
