#include <QtCore/QDebug>
#include <QtCore/QStandardPaths>
#include <QtGui/QImageReader>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QAbstractSlider>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include "application.h"
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
    setWindowTitle(app().name());
    ui_->steps->setLabel("Steps");
    ui_->shapes->setLabel("Shapes");
    ui_->score->setLabel("Score");

    connect(ui_->openButton, &QAbstractButton::clicked, this, &MainWindow::load);
    connect(ui_->saveButton, &QAbstractButton::clicked, this, &MainWindow::save);
    connect(ui_->startButton, &QAbstractButton::clicked, this, &MainWindow::start);
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

        start();
    }
}

void MainWindow::save() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save Image", "", "SVG Image (*.svg);;All Files (*)");

    if (!fileName.isEmpty()) {
        QFile file(fileName);

        if (file.open(QIODevice::WriteOnly)) {
            file.write(images_[ui_->imageSlider->value()].svgData_);
            file.close();
        } else {
            displayError(QString("Cannot open file for writing: %1").arg(fileName));
        }
    }
}

void MainWindow::start()
{  
    images_.clear();
    ui_->imageWidget->setBaseImage(startImage_);

    if (app().settings().primitiveBinPath().isEmpty()) {
        displayError("Cannot find 'primitive' executable.");
        return;
    }
    QString path = QStandardPaths::findExecutable(app().settings().primitiveBinPath());
    if (path.isEmpty()) {
        displayError(QString("Cannot determine path to selected 'primitive' executable:\n%1").arg(app().settings().primitiveBinPath()));
        return;
    }

    process_->start(startImage_);
    
    ui_->openButton->setEnabled(false);
    ui_->saveButton->setEnabled(false);
    ui_->startButton->setEnabled(false);
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
            start();
        }
    }
}

void MainWindow::renderingFinished()
{
    ui_->openButton->setEnabled(true);
    ui_->saveButton->setEnabled(!images_.isEmpty());
    ui_->startButton->setEnabled(!startImage_.isNull());
    ui_->stopButton->setEnabled(false);
    ui_->settingsButton->setEnabled(true);

    ui_->imageSlider->setEnabled(true);

    ui_->imageSlider->setRange(0, images_.size() - 1);
    ui_->imageSlider->setValue(images_.size() - 1);
}


void MainWindow::renderingIntermediate(QByteArray svgData, int steps, int shapes, double score)
{
    images_ << ImageInfo{svgData, steps, shapes, score};
    showImage(images_.size() - 1);
}

void MainWindow::displayError(QString message)
{
    QMessageBox::critical(this, app().name(), message);
}

void MainWindow::showImage(int index)
{
    if ((index < 0) || (index >= images_.size())) {
        return;
    }
    const auto& info = images_[index];
    ui_->imageWidget->showSvgImage(info.svgData_);

    ui_->steps->clearValue();
    ui_->shapes->clearValue();
    ui_->score->clearValue();

    if (info.steps_ >= 0) {
        ui_->steps->setValue(info.steps_);
    }
    if (info.shapes_ >= 0) {
        ui_->shapes->setValue(info.shapes_);
    }
    if (info.score_ >= 0) {
        ui_->score->setValue(info.score_);
    }
}
