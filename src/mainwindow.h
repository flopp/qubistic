#pragma once

#include <QtGui/QPixmap>
#include <QtWidgets/QWidget>
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
    void displayError(QString message);

private:
    void runRenderer();

private slots:
    void renderingFinished();
    void renderingIntermediate(QByteArray svgData, int steps, int shapes, double score);
    void showImage(int index);

private:
    Ui_MainWindow* ui_{nullptr};
    RenderProcess* process_{nullptr};
    QPixmap startImage_;
    
    struct ImageInfo {
        ImageInfo(QByteArray svgData, int steps, int shapes, double score) :
            svgData_{svgData},
            steps_{steps},
            shapes_{shapes},
            score_{score}
        {}
        
        QPixmap image_;
        QByteArray svgData_;
        int steps_;
        int shapes_;
        double score_;
    };
    QList<ImageInfo> images_;
};
