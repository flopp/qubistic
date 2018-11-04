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
#include "statuswidget.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow{parent},
    ui_{new Ui_MainWindow},
    process_{new RenderProcess{this}}
{
    ui_->setupUi(this);
    setWindowTitle(app().name());

    // setup toolbar
    openAction_ = ui_->toolBar->addAction(loadIcon("document-open"), "Open", this, &MainWindow::load);
    saveAction_ = ui_->toolBar->addAction(loadIcon("document-save"), "Save", this, &MainWindow::save);
    startAction_ = ui_->toolBar->addAction(loadIcon("media-playback-start"), "Start", this, &MainWindow::start);
    stopAction_ = ui_->toolBar->addAction(loadIcon("media-playback-stop"), "Stop", this, &MainWindow::stop);
    settingsAction_ = ui_->toolBar->addAction(loadIcon("document-properties"), "Settings", this, &MainWindow::showSettings);

    QWidget* spacer{new QWidget{this}};
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui_->toolBar->addWidget(spacer);

    steps_ = new StatusWidget(this, "Steps");
    shapes_ = new StatusWidget(this, "Shapes");
    score_ = new StatusWidget(this, "Score");
    ui_->toolBar->addWidget(steps_);
    ui_->toolBar->addWidget(shapes_);
    ui_->toolBar->addWidget(score_);

    connect(ui_->imageSlider, &QAbstractSlider::valueChanged, this, &MainWindow::showImage);

    connect(process_, &RenderProcess::finished, this, &MainWindow::renderingFinished);
    connect(process_, &RenderProcess::intermediate, this, &MainWindow::renderingIntermediate);

    connect(process_, &RenderProcess::error, this, &MainWindow::displayError);
    connect(ui_->imageWidget, &ImageWidget::error, this, &MainWindow::displayError);

    updateButtons(false);
}

MainWindow::~MainWindow() {
    delete ui_;
}

void MainWindow::load() {
    stop();

    QString fileName{QFileDialog::getOpenFileName(this, "Open Image", "", "Image Files (*.png *.jpg *.JPG *.jpeg);;All Files (*)")};

    if (!fileName.isEmpty()) {
        // use QImageReader with 'autoTransform' to automatically apply image rotation based on the EXIF data.
        QImageReader reader(fileName);
        reader.setAutoTransform(true);
        QImage image = reader.read();
        startImage_ = QPixmap::fromImage(image);

        start();
    }
}

void MainWindow::save() {
    QString fileName{QFileDialog::getSaveFileName(this, "Save Image", "", "SVG Image (*.svg);;All Files (*)")};

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

void MainWindow::start() {
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

    updateButtons(true);

    ui_->imageSlider->setEnabled(false);
}

void MainWindow::stop() {
    process_->stop();
}

void MainWindow::showSettings() {
    SettingsDialog dialog{this};
    dialog.toggleApplyAndRun(!startImage_.isNull());
    if (dialog.exec() == QDialog::Accepted) {
        if (dialog.applyAndRun()) {
            start();
        }
    }
}

void MainWindow::renderingFinished() {
    updateButtons(false);
    ui_->imageSlider->setEnabled(true);
    ui_->imageSlider->setRange(0, images_.size() - 1);
    ui_->imageSlider->setValue(images_.size() - 1);
}


void MainWindow::renderingIntermediate(QByteArray svgData, int steps, int shapes, double score) {
    images_ << ImageInfo{svgData, steps, shapes, score};
    showImage(images_.size() - 1);
}

void MainWindow::displayError(QString message) {
    QMessageBox::critical(this, app().name(), message);
}

void MainWindow::showImage(int index) {
    if ((index < 0) || (index >= images_.size())) {
        return;
    }
    const auto& info = images_[index];
    ui_->imageWidget->showSvgImage(info.svgData_);

    steps_->clearValue();
    shapes_->clearValue();
    score_->clearValue();

    if (info.steps_ >= 0) {
        steps_->setValue(info.steps_);
    }
    if (info.shapes_ >= 0) {
        shapes_->setValue(info.shapes_);
    }
    if (info.score_ >= 0) {
        score_->setValue(info.score_);
    }
}

QIcon MainWindow::loadIcon(QString name) const {
    if (QIcon::hasThemeIcon(name)) {
        return QIcon::fromTheme(name);
    }
    return QIcon{};
}

void MainWindow::updateButtons(bool running) {
    if (running) {
        openAction_->setEnabled(false);
        saveAction_->setEnabled(false);
        startAction_->setEnabled(false);
        stopAction_->setEnabled(true);
        settingsAction_->setEnabled(false);
    } else {
        openAction_->setEnabled(true);
        saveAction_->setEnabled(!images_.isEmpty());
        startAction_->setEnabled(!startImage_.isNull());
        stopAction_->setEnabled(false);
        settingsAction_->setEnabled(true);
    }
}
