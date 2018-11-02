#include <type_traits>
#include <QtCore/QDebug>
#include <QtCore/QRegularExpression>
#include <QtCore/QTemporaryDir>
#include <QtGui/QPixmap>
#include "renderprocess.h"
#include "settings.h"

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

void RenderProcess::start(const QPixmap& image)
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
 
    lastImage_.clear();
    lastShapes_ = -1;
    lastScore_ = 0;
    
    QStringList processArgs;
    processArgs << "-v";
    processArgs << "-nth" << "1";
    processArgs << "-m" << QString::number(static_cast<std::underlying_type<ShapeType>::type>(settings().shapeType()));
    processArgs << "-i" << tempDir_->filePath("start.png");
    processArgs << "-o" << tempDir_->filePath("result%d.svg");
    switch (settings().targetType()) {
        case TargetType::Shapes:
            processArgs << "-n" << QString::number(settings().targetShapes());
            break;
        case TargetType::Score:
            processArgs << "-n" << "10000";
            break;
    }

    killed_ = false;
    process_->start(settings().primitiveBinPath(), processArgs);
}

void RenderProcess::stop()
{
    if (process_->state() != QProcess::NotRunning) {
        killed_ = true;
        process_->kill();
        process_->waitForFinished();
    }
}

void RenderProcess::processFinished(int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/)
{
    if (!killed_) {
        readProcessOutput();
        if (!lastImage_.isEmpty()) {
            loadLastImage();
        }
    }
    emit finished();
}

void RenderProcess::readProcessOutput()
{
    if (killed_) {
        return;
    }

    QRegularExpression re_status("^(\\d+): t=.*, score=([0-9\\.]+),");
    QRegularExpression re_write("^writing\\s+(.*\\.svg)");
    QRegularExpressionMatch match;
    while (process_->canReadLine()) {
        QString line = QString::fromUtf8(process_->readLine());
        
        match = re_write.match(line);
        if (match.hasMatch()) {
            lastImage_ = match.captured(1);
            continue;
        } else {
            if (!lastImage_.isEmpty()) {
                loadLastImage();
                if ((settings().targetType() == TargetType::Score) && (lastScore_ > settings().targetScore())) {
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


void RenderProcess::loadLastImage()
{
    if (lastImage_.isEmpty()) {
        return;
    }

    QFile file(lastImage_);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "failed to load" << lastImage_;
    } else {
        emit intermediate(file.readAll(), lastShapes_, lastScore_);
    }
    lastImage_.clear();
}
