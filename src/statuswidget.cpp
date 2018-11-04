#include "statuswidget.h"
#include "ui_statuswidget.h"

StatusWidget::StatusWidget(QWidget* parent, QString label) :
    QWidget{parent},
    ui_{new Ui_StatusWidget}
{
    ui_->setupUi(this);
    setLabel(label);
}

StatusWidget::~StatusWidget()
{
    delete ui_;
}

void StatusWidget::setLabel(const QString& label) {
    ui_->label->setText(label);
}

void StatusWidget::clearValue() {
    ui_->value->setText("n/a");
}

void StatusWidget::setValue(int value) {
    ui_->value->setText(QString::number(value));
}

void StatusWidget::setValue(double value) {
    ui_->value->setText(QString::number(value, 'f', 2));
}
