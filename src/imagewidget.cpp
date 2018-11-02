#include <QtGui/QPainter>
#include "imagewidget.h"

ImageWidget::ImageWidget(QWidget* parent) :
    QWidget{parent}
{}

ImageWidget::~ImageWidget()
{
}

void ImageWidget::showImage(const QPixmap& image)
{
    baseImage_ = image;
    resizedImage_ = baseImage_.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    update();
}

void ImageWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter p(this);
    p.fillRect(0, 0, width(), height(), Qt::black);

    if (!baseImage_.isNull())
    {
        const int w = resizedImage_.width();
        const int h = resizedImage_.height();
        const int x = (width() - w) / 2;
        const int y = (height() - h) / 2;
        p.drawPixmap(x, y, resizedImage_, 0, 0, w, h);
    }
}

void ImageWidget::resizeEvent(QResizeEvent* /*event*/)
{
    if (baseImage_.isNull())
    {
        return;
    }
    if ((resizedImage_.width() == width()) && (resizedImage_.height() <= height()))
    {
        return;
    }
    if ((resizedImage_.height() == height()) && (resizedImage_.width() <= width()))
    {
        return;
    }
    resizedImage_ = baseImage_.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
}
