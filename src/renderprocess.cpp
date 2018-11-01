#include <type_traits>
#include <QtCore/QDebug>
#include <QtCore/QRegularExpression>
#include <QtCore/QTemporaryDir>
#include "renderprocess.h"

RenderProcess::RenderProcess(QObject* parent) :
    QObject(parent),
    process_{new QProcess{this}}
{
    connect(process_, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
    connect(process_, &QProcess::readyReadStandardOutput, this, &RenderProcess::readProcessOutput);
}

RenderProcess::~RenderProcess()
{
    stop();
    delete tempDir_;
}

void RenderProcess::start(const QPixmap& image, Configuration config)
{
    stop();

    delete tempDir_;    
    tempDir_ = new QTemporaryDir;
    if (!tempDir_->isValid()) {
        // cannot create temp dir!
        return;
    }
    
    QFile file(tempDir_->filePath("start.png"));
    file.open(QIODevice::WriteOnly);
    image.save(&file, "PNG");
    file.close();
 
    config_ = config;

    lastImage_.clear();
    lastShapes_ = -1;
    lastScore_ = 0;
    
    QStringList processArgs;
    processArgs << "-v";
    processArgs << "-nth" << "1";
    processArgs << "-m" << QString::number(static_cast<std::underlying_type<Configuration::ShapeType>::type>(config.shapeType));
    processArgs << "-i" << tempDir_->filePath("start.png");
    processArgs << "-o" << tempDir_->filePath("result%d.png");
    if (config_.targetType == Configuration::TargetType::Shapes) {
        processArgs << "-n" << QString::number(config.targetShapes);
    } else {
        processArgs << "-n" << "10000";
    }
    QString exe = "gopath/bin/primitive";
    process_->start(exe, processArgs);
}

void RenderProcess::stop()
{
    if (process_->state() != QProcess::NotRunning)
    {
        process_->kill();
        process_->waitForFinished();
    }
}

void RenderProcess::processFinished(int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/)
{
    readProcessOutput();
    if (!lastImage_.isEmpty()) {
        QPixmap image;
        if (image.load(lastImage_, "png")) {
            emit intermediate(image, lastShapes_, lastScore_);
        } else {
            qDebug() << "failed to load" << lastImage_;
        }
        lastImage_.clear();
    }
    emit finished();
}

void RenderProcess::readProcessOutput()
{
    QRegularExpression re_status("^(\\d+): t=.*, score=([0-9\\.]+),");
    QRegularExpression re_write("^writing\\s+(.*\\.png)");
    QRegularExpressionMatch match;
    while (process_->canReadLine()) {
        QString line = QString::fromUtf8(process_->readLine());
        
        match = re_write.match(line);
        if (match.hasMatch()) {
            lastImage_ = match.captured(1);
            continue;
        } else {
            if (!lastImage_.isEmpty()) {
                QPixmap image;
                if (image.load(lastImage_, "PNG")) {
                    emit intermediate(image, lastShapes_, lastScore_);
                } else {
                    qDebug() << "failed to load" << lastImage_;
                }
                lastImage_.clear();
                
                if ((config_.targetType == Configuration::TargetType::Score) && (lastScore_ > config_.targetScore)) {
                    stop();
                }
            }
        }
        
        match = re_status.match(line);
        if (match.hasMatch()) {
            lastShapes_ = match.captured(1).toInt();
            lastScore_ = 100.0 * (1.0 - match.captured(2).toFloat());
            continue;
        }
    }
}
