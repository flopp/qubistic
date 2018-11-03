#pragma once

#include <QtCore/QDir>
#include <QtCore/QObject>
#include <QtCore/QProcess>
#include <QtCore/QRegularExpression>

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
    void intermediate(QByteArray svgData, int steps, int shapes, double score);
    void error(QString message);

private slots:
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void errorOccurred(QProcess::ProcessError error);
    void readProcessOutput();

private:
    void loadLastImage();

private:
    QTemporaryDir* tempDir_{nullptr};
    QProcess* process_{nullptr};

    QRegularExpression re_reading;
    QRegularExpression re_writing;
    QRegularExpression re_status0;
    QRegularExpression re_status;
    QRegularExpression re_parameters;

    bool killed_{false};
    QString lastImage_;
    int lastSteps_{0};
    int lastShapes_{0};
    double lastScore_{0};
};
