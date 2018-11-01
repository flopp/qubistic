#pragma once

#include <QtGui/QPixmap>
#include <QtWidgets/QWidget>
#include "configuration.h"
class RenderProcess;
class Ui_MainWindow;

class MainWindow: public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    virtual ~MainWindow();

public slots:
    void load();
    void stop();
    void showSettings();

private:
    void runRenderer();

private slots:
    void renderingFinished();
    void renderingIntermediate(QPixmap image, int shapes, double score);
    void renderingAborted();
    void showImage(int index);

private:
    Ui_MainWindow* ui_{nullptr};
    RenderProcess* process_{nullptr};
    QPixmap startImage_;
    
    struct ImageInfo {
        ImageInfo(QPixmap image, int shapes, double score) :
            image_{image},
            shapes_{shapes},
            score_{score}
        {}
        
        QPixmap image_;
        int shapes_;
        double score_;
    };
    QList<ImageInfo> images_;
    Configuration config_;
};
