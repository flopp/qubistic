#pragma once

#include <QtCore/QDir>
#include <QtCore/QObject>
#include <QtCore/QProcess>

class QPixmap;
class QTemporaryDir;

class RenderProcess: public QObject
{
    Q_OBJECT
    
public:
    explicit RenderProcess(QObject* parent = nullptr);
    virtual ~RenderProcess();
    
public slots:
    void start(const QPixmap& startImage);
    void stop();

signals:
    void finished();
    void intermediate(QByteArray svgData, int shapes, double score);
    void aborted();

private slots:
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void readProcessOutput();

private:
    void loadLastImage();

private:
    QTemporaryDir* tempDir_{nullptr};
    QProcess* process_{nullptr};

    bool killed_{false};
    QString lastImage_;
    int lastShapes_{0};
    double lastScore_{0};
};
