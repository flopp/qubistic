#pragma once

#include <QtGui/QPixmap>
#include <QtWidgets/QWidget>

class ImageWidget: public QWidget
{
    Q_OBJECT

public:
    explicit ImageWidget(QWidget* parent = nullptr);
    virtual ~ImageWidget();
    
    void showImage(const QPixmap& image);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    QPixmap baseImage_;
    QPixmap resizedImage_;
};
