#pragma once

#include <QtGui/QPixmap>
#include <QtWidgets/QAction>
#include <QtWidgets/QMainWindow>
class RenderProcess;
class StatusWidget;
class Ui_MainWindow;

class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    virtual ~MainWindow();

public slots:
    void load();
    void save();
    void start();
    void stop();

    void showSettings();
    void displayError(QString message);

private slots:
    void renderingFinished();
    void renderingIntermediate(QByteArray svgData, int steps, int shapes, double score);
    void showImage(int index);

private:
    QIcon loadIcon(QString name) const;
    void updateButtons(bool running);

    Ui_MainWindow* ui_{nullptr};

    QAction* openAction_{nullptr};
    QAction* saveAction_{nullptr};
    QAction* startAction_{nullptr};
    QAction* stopAction_{nullptr};
    QAction* settingsAction_{nullptr};

    StatusWidget* steps_{nullptr};
    StatusWidget* shapes_{nullptr};
    StatusWidget* score_{nullptr};

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
