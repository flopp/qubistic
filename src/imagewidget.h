#pragma once

#include <QtGui/QPixmap>
#include <QtWidgets/QWidget>
class QSvgRenderer;

class ImageWidget: public QWidget
{
    Q_OBJECT

public:
    explicit ImageWidget(QWidget* parent = nullptr);
    virtual ~ImageWidget();

    void showImage(const QPixmap& image);
    void showSvgImage(const QByteArray& imageData);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    QPixmap baseImage_;
    QPixmap resizedImage_;

    QSvgRenderer* svgRenderer_{nullptr};
};
