#pragma once

#include <QtCore/QDir>
#include <QtCore/QObject>
#include <QtCore/QProcess>
#include <QtGui/QPixmap>
#include "configuration.h"

class QTemporaryDir;

class RenderProcess: public QObject
{
    Q_OBJECT
    
public:
    explicit RenderProcess(QObject* parent = nullptr);
    virtual ~RenderProcess();
    
public slots:
    void start(const QPixmap& startImage, Configuration config);
    void stop();

signals:
    void finished();
    void intermediate(QPixmap image, int shapes, double score);
    void aborted();

private slots:
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void readProcessOutput();

private:
    void cleanup();

private:
    QTemporaryDir* tempDir_{nullptr};
    QProcess* process_{nullptr};

    Configuration config_;

    QString lastImage_;
    int lastShapes_;
    double lastScore_;
};