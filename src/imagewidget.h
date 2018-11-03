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

    void setBaseImage(const QPixmap& image);
    void showSvgImage(const QByteArray& imageData);

signals:
    void error(QString message);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QPixmap baseImage_;
    QPixmap resizedImage_;

    QSvgRenderer* svgRenderer_{nullptr};

    bool hasSvg_{false};
    bool baseShown_{true};
};
