#include <type_traits>
#include <QtCore/QDebug>
#include <QtCore/QTemporaryDir>
#include <QtGui/QPixmap>
#include "renderprocess.h"
#include "settings.h"

RenderProcess::RenderProcess(QObject* parent) :
    QObject(parent),
    process_{new QProcess{this}}
{
    process_->setProcessChannelMode(QProcess::MergedChannels);

    connect(process_, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        [=](int exitCode, QProcess::ExitStatus exitStatus){
            this->processFinished(exitCode, exitStatus);
    });
    connect(process_, &QProcess::errorOccurred, this, &RenderProcess::errorOccurred);
    connect(process_, &QProcess::readyReadStandardOutput, this, &RenderProcess::readProcessOutput);

    re_reading.setPattern("^reading\\s+(.*)$");
    re_writing.setPattern("^writing\\s+(.*)$");
    re_status0.setPattern("^0:\\s+t=([0-9\\.]+),\\s+score=([0-9\\.]+)$");
    re_status.setPattern("^(\\d+):\\s+t=([0-9\\.]+),\\s+score=([0-9\\.]+),\\s+n=([0-9]+),\\s+n/s=([0-9\\.]+[a-zA-Z])$");
    re_parameters.setPattern("^count=([^,]+),\\s+mode=([^,]+),\\s+alpha=([^,]+),\\s+repeat=(\\d+)$");

    Q_ASSERT(re_reading.isValid());
    Q_ASSERT(re_writing.isValid());
    Q_ASSERT(re_status0.isValid());
    Q_ASSERT(re_status.isValid());
    Q_ASSERT(re_parameters.isValid());
}

RenderProcess::~RenderProcess()
{
    stop();
    delete tempDir_;
}

void RenderProcess::start(const QPixmap& image)
{
    stop();
    killed_ = false;

    delete tempDir_;    
    tempDir_ = new QTemporaryDir;
    if (!tempDir_->isValid()) {
        emit error("Cannot create temporary directory.");
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

    process_->start(settings().primitiveBinPath(), processArgs);
}

void RenderProcess::stop()
{
    killed_ = true;
    if (process_->state() != QProcess::NotRunning) {
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

void RenderProcess::errorOccurred(QProcess::ProcessError errorCode)
{
    switch (errorCode) {
        case QProcess::FailedToStart:
            emit error("Could not start 'primitive'.");
            break;
        case QProcess::Crashed:
            if (!killed_) {
                emit error("'primitive' unexpectedly crashed.");
            }
            break;
        case QProcess::Timedout:
            emit error("'primitive' unexpectedly timed out.");
            break;
        case QProcess::WriteError:
            emit error("'primitive' write error.");
            break;
        case QProcess::ReadError:
            emit error("'primitive' read error.");
            break;
        case QProcess::UnknownError:
            emit error("'primitive' unknown error.");
            break;
    }
}

void RenderProcess::readProcessOutput()
{
    if (killed_) {
        return;
    }

    QRegularExpressionMatch match;
    while (process_->canReadLine()) {
        QString line = QString::fromUtf8(process_->readLine()).trimmed();

        match = re_reading.match(line);
        if (match.hasMatch()) {
            Q_ASSERT(lastImage_.isEmpty());
            continue;
        }

        match = re_status0.match(line);
        if (match.hasMatch()) {
            Q_ASSERT(lastImage_.isEmpty());
            lastShapes_ = 0;
            lastScore_ = 100.0 * (1.0 - match.captured(1).toFloat());
            continue;
        }

        match = re_parameters.match(line);
        if (match.hasMatch()) {
            Q_ASSERT(lastImage_.isEmpty());
            continue;
        }

        match = re_writing.match(line);
        if (match.hasMatch()) {
            lastImage_ = match.captured(1);
            continue;
        }

        match = re_status.match(line);
        if (match.hasMatch()) {
            if (!lastImage_.isEmpty()) {
                loadLastImage();
                if ((settings().targetType() == TargetType::Score) && (lastScore_ > settings().targetScore())) {
                    stop();
                }
            }
            lastShapes_ = match.captured(1).toInt();
            lastScore_ = 100.0 * (1.0 - match.captured(3).toFloat());
            continue;
        }

        emit error(QString("Unexpected output:\n%1").arg(line));

        stop();
        break;
    }
}

void RenderProcess::loadLastImage()
{
    if (killed_) {
        return;
    }

    if (lastImage_.isEmpty()) {
        return;
    }

    QFile file(lastImage_);
    if (!file.open(QIODevice::ReadOnly)) {
        emit error(QString("Failed to load image file:\n%1").arg(lastImage_));
    } else {
        emit intermediate(file.readAll(), lastShapes_, lastScore_);
    }
    lastImage_.clear();
}
