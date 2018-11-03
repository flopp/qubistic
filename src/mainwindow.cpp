#include <QtCore/QDebug>
#include <QtCore/QStandardPaths>
#include <QtGui/QGuiApplication>
#include <QtGui/QImageReader>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QAbstractSlider>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include "mainwindow.h"
#include "renderprocess.h"
#include "settings.h"
#include "settingsdialog.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) :
    QWidget{parent},
    ui_{new Ui_MainWindow},
    process_{new RenderProcess{this}}
{
    ui_->setupUi(this);

    connect(ui_->loadButton, &QAbstractButton::clicked, this, &MainWindow::load);
    connect(ui_->stopButton, &QAbstractButton::clicked, this, &MainWindow::stop);
    connect(ui_->settingsButton, &QAbstractButton::clicked, this, &MainWindow::showSettings);
    connect(ui_->imageSlider, &QAbstractSlider::valueChanged, this, &MainWindow::showImage);
    
    connect(process_, &RenderProcess::finished, this, &MainWindow::renderingFinished);
    connect(process_, &RenderProcess::intermediate, this, &MainWindow::renderingIntermediate);

    connect(process_, &RenderProcess::error, this, &MainWindow::displayError);
    connect(ui_->imageWidget, &ImageWidget::error, this, &MainWindow::displayError);
}

MainWindow::~MainWindow()
{
    delete ui_;
}

void MainWindow::load()
{
    stop();

    QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", "Image Files (*.png *.jpg *.JPG *.jpeg);;All Files (*)");
    
    if (!fileName.isEmpty())
    {
        // use QImageReader with 'autoTransform' to automatically apply image rotation based on the EXIF data.
        QImageReader reader(fileName);
        reader.setAutoTransform(true);
        QImage image = reader.read();
        startImage_ = QPixmap::fromImage(image);

        runRenderer();
    }
}

void MainWindow::runRenderer()
{  
    images_.clear();
    ui_->imageWidget->showImage(startImage_);

    if (settings().primitiveBinPath().isEmpty()) {
        displayError("Cannot find 'primitive' executable.");
        return;
    }
    QString path = QStandardPaths::findExecutable(settings().primitiveBinPath());
    if (path.isEmpty()) {
        displayError(QString("Cannot determine path to selected 'primitive' executable:\n%1").arg(settings().primitiveBinPath()));
        return;
    }

    process_->start(startImage_);
    
    ui_->loadButton->setEnabled(false);
    ui_->stopButton->setEnabled(true);
    ui_->settingsButton->setEnabled(false);
    ui_->imageSlider->setEnabled(false);
}

void MainWindow::stop()
{
    process_->stop();
}

void MainWindow::showSettings()
{
    SettingsDialog dialog{this};
    dialog.toggleApplyAndRun(!startImage_.isNull());
    if (dialog.exec() == QDialog::Accepted) {
        if (dialog.applyAndRun()) {
            runRenderer();
        }
    }
}

void MainWindow::renderingFinished()
{
    ui_->loadButton->setEnabled(true);
    ui_->stopButton->setEnabled(false);
    ui_->settingsButton->setEnabled(true);
    ui_->imageSlider->setEnabled(true);

    images_ << ImageInfo{startImage_, QByteArray{}, -1, 100.0};
    ui_->imageSlider->setRange(0, images_.size() - 1);
    ui_->imageSlider->setValue(images_.size() - 2);
}


void MainWindow::renderingIntermediate(QByteArray svgData, int shapes, double score)
{
    images_ << ImageInfo{QPixmap{}, svgData, shapes, score};
    showImage(images_.size() - 1);
}

void MainWindow::displayError(QString message)
{
    QMessageBox::critical(this, qApp->applicationName(), message);
}

void MainWindow::showImage(int index)
{
    if ((index < 0) || (index >= images_.size())) {
        return;
    }
    const auto& info = images_[index];
    
    if (!info.image_.isNull()) {
        ui_->imageWidget->showImage(info.image_);
    } else {
        ui_->imageWidget->showSvgImage(info.svgData_);
    }

    if (info.shapes_ < 0) {
        ui_->shapesLabel->setText("\u221e");
    } else {
        ui_->shapesLabel->setText(QString::number(info.shapes_));
    }
    ui_->scoreLabel->setText(QString::number(info.score_, 'f', 2));
}
