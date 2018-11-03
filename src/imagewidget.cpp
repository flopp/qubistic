#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtSvg/QSvgRenderer>
#include "imagewidget.h"

ImageWidget::ImageWidget(QWidget* parent) :
    QWidget{parent},
    svgRenderer_{new QSvgRenderer{this}}
{}

ImageWidget::~ImageWidget()
{}

void ImageWidget::setBaseImage(const QPixmap& image)
{
    hasSvg_ = false;
    baseShown_ = false;
    baseImage_ = image;
    resizedImage_ = baseImage_.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    update();
}

void ImageWidget::showSvgImage(const QByteArray& imageData)
{
    hasSvg_ = true;
    //baseImage_ = resizedImage_ = QPixmap();
    if (!svgRenderer_->load(imageData)) {
        hasSvg_ = false;
        emit error("Cannot load SVG data.");
    }
    if (!baseShown_) {
        update();
    }
}

void ImageWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter p(this);
    p.setWindow(rect());
    p.fillRect(rect(), Qt::black);

    if (baseShown_ || !hasSvg_) {
        if (!baseImage_.isNull())
        {
            const int w = resizedImage_.width();
            const int h = resizedImage_.height();
            const int x = (width() - w) / 2;
            const int y = (height() - h) / 2;
            p.drawPixmap(x, y, resizedImage_, 0, 0, w, h);
        }
    } else if (svgRenderer_->isValid()) {
        QRect viewbox = svgRenderer_->viewBox();
        double scale = 1;
        int x = 0;
        int y = 0;
        int w = viewbox.width();
        int h = viewbox.height();
        if ((width() * h) > (height() * w))
        {
            scale = height() / static_cast<double>(h);
            x = (width() - (scale * w)) / 2;
        }
        else
        {
            scale = width() / static_cast<double>(w);
            y = (height() - (scale * h)) / 2;
        }
        p.translate(x, y);
        p.scale(scale, scale);
        p.setClipRect(viewbox);
        svgRenderer_->render(&p, viewbox);
    }
}

void ImageWidget::resizeEvent(QResizeEvent* /*event*/)
{
    if (!baseImage_.isNull())
    {
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
}

void ImageWidget::mousePressEvent(QMouseEvent* /*event*/) {
    if (!baseShown_) {
        baseShown_ = true;
        update();
    }
}

void ImageWidget::mouseReleaseEvent(QMouseEvent* /*event*/) {
    if (baseShown_) {
        baseShown_ = false;
        update();
    }
}
