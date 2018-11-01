#include <QtCore/QDebug>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QFileDialog>

#include "mainwindow.h"
#include "renderprocess.h"
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
    connect(process_, &RenderProcess::aborted, this, &MainWindow::renderingAborted);
    connect(process_, &RenderProcess::intermediate, this, &MainWindow::renderingIntermediate);
}

MainWindow::~MainWindow()
{
    delete ui_;
}

void MainWindow::load()
{
    stop();

    QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.jpg *.JPG *.jpeg)" );
    
    if (fileName != "")
    {
        startImage_.load(fileName);
        startImage_ = startImage_.scaled(QSize{1024, 1024}, Qt::KeepAspectRatio);
        runRenderer();
    }
}

void MainWindow::runRenderer()
{  
    images_.clear();
    ui_->imageLabel->setPixmap(startImage_);
    process_->start(startImage_, config_);
    
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
    dialog.setConfig(config_);
    if (dialog.exec() == QDialog::Accepted)
    {
        config_ = dialog.config();
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

    images_ << ImageInfo{startImage_, -1, 100.0};
    ui_->imageSlider->setRange(0, images_.size() - 1);
    ui_->imageSlider->setValue(images_.size() - 2);
}

void MainWindow::renderingIntermediate(QPixmap image, int shapes, double score)
{
    images_ << ImageInfo{image, shapes, score};
    showImage(images_.size() - 1);
}

void MainWindow::renderingAborted()
{
    ui_->loadButton->setEnabled(true);
    ui_->stopButton->setEnabled(false);
    ui_->settingsButton->setEnabled(true);
    ui_->imageSlider->setEnabled(false);
}

void MainWindow::showImage(int index)
{
    if ((index < 0) || (index >= images_.size()))
    {
        return;
    }
    const auto& info = images_[index];
    ui_->imageLabel->setPixmap(info.image_);
    if (info.shapes_ < 0) {
        ui_->shapesLabel->setText("\u221e");
    } else {
        ui_->shapesLabel->setText(QString::number(info.shapes_));
    }
    ui_->scoreLabel->setText(QString::number(info.score_, 'f', 2));
    
}
