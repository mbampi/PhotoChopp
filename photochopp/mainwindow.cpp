#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QIntValidator>
#include <QPixmap>
#include <QMessageBox>
#include <QPainter>
#include <QPointer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showOriginalImage(){
    QImage img = this->editor.getOriginalImage();
    const int width = this->ui->originalImageView->width();
    const int height = this->ui->originalImageView->height();

    const QPixmap pixmap = QPixmap::fromImage(img).scaled(width, height, Qt::KeepAspectRatio);
    this->ui->originalImageView->setPixmap(pixmap);
}

void MainWindow::showEditedImage(){
    QImage img = this->editor.getEditedImage();
    const int width = this->ui->editedImageView->width();
    const int height = this->ui->editedImageView->height();

    const QPixmap pixmap = QPixmap::fromImage(img).scaled(width, height, Qt::KeepAspectRatio);
    this->ui->editedImageView->setPixmap(pixmap);
}

void MainWindow::on_actionLoadImage_triggered()
{
    QString selfilter = tr("Image Files (*.png *.jpg *.jpeg *.bmp)");
    QString filename = QFileDialog::getOpenFileName(this, "Load Image",
                                                    QDir::currentPath(),
                                                    tr("Image Files (*.png *.jpg *.bmp)"),
                                                    &selfilter);

    if(!filename.isNull())
    {
        this->editor.loadImage(filename);
        this->showOriginalImage();
        this->showEditedImage();
        this->ui->actionSaveImage->setEnabled(true);
        this->ui->loadImageButton->setVisible(false);
    } else {
        printf("Error on on_actionLoadImage_triggered: filename is null");
    }
}


void MainWindow::on_actionSaveImage_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save image", QDir::currentPath(),
                                                    "Image Files (*.png *.jpg *.bmp)",
                                                    new QString("JPEG Image (*.jpg)"));
    if(!filename.isNull()) {
        if (!filename.endsWith(".jpg"))
            filename.append(".jpg");
        this->editor.saveImage(filename);
    } else
        printf("Error on on_actionSaveImage_triggered: filename is null");
}

void MainWindow::on_verticalFlipButton_clicked()
{
    this->editor.flipVertically();
    this->showEditedImage();
}

void MainWindow::on_horizontalFilpButton_clicked()
{
    this->editor.flipHorizontally();
    this->showEditedImage();
}

void MainWindow::on_grayscaleButton_clicked()
{
    QImage image = this->editor.getEditedImage();
    image = this->editor.toGrayscale(image);
    this->editor.setEditedImage(image);
    this->showEditedImage();
}

void MainWindow::on_revertButton_clicked()
{
    this->editor.revertImageChanges();
    this->showEditedImage();
}

void MainWindow::on_quantize_clicked()
{
    int tones = this->ui->tonesSpinBox->value();

    if (tones < 2 || tones > 256)
        QMessageBox::information(0, "Error!", "Grayscale tones should be between 2 e 256", QMessageBox::Ok);
    else {
        this->editor.quantize(tones);
        this->showEditedImage();
    }
}

void MainWindow::on_loadImageButton_clicked()
{
    MainWindow::on_actionLoadImage_triggered();
}

void MainWindow::showHistogram(std::vector<int> histogram) {
    auto max_histogram = std::max_element(histogram.begin(), histogram.end());

    QPixmap histogramChart(256, 256);
    histogramChart.fill(Qt::darkGray);
    QPainter painter(&histogramChart);
    painter.setPen(Qt::red);

    for (int i = 0; i < 256; i++) {
        double lineHeight = std::round(histogram[i] * 1.0 / *max_histogram * 255);
        painter.drawLine(i, 255, i, 255 - lineHeight);
    }

    QPointer<QLabel> histogramWindow = new QLabel();
    histogramWindow->setWindowTitle("Gayscale Histogram");
    histogramWindow->setPixmap(histogramChart);
    histogramWindow->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    histogramWindow->adjustSize();
    histogramWindow->show();
}

void MainWindow::on_histogramButton_clicked()
{
    QImage image = this->editor.getEditedImage().copy();
    if (!image.isGrayscale())
        image = this->editor.toGrayscale(image);

    auto histogram = this->editor.generateGrayscaleHistogram(image);
    this->showHistogram(histogram);
}

void MainWindow::on_brightnessButton_clicked()
{
    int brightLevel = this->ui->brightlevelSpinBox->value();

    this->editor.adjustBrightness(brightLevel);
    this->showEditedImage();
}

void MainWindow::on_contrastButton_clicked()
{
    double contrastFactor = this->ui->contrastFactorSpinBox->value();

    this->editor.adjustContrast(contrastFactor);
    this->showEditedImage();
}

void MainWindow::on_negativeButton_clicked()
{
    this->editor.toNegative();
    this->showEditedImage();
}

void MainWindow::on_histogramEqualizationButton_clicked()
{
    this->editor.histogramEqualization();
    this->showEditedImage();
}

void MainWindow::on_histogramMatchingButton_clicked()
{
    QString selfilter = tr("Image Files (*.png *.jpg *.jpeg *.bmp)");
    QString filename = QFileDialog::getOpenFileName(this, "Load Image",
                                                    QDir::currentPath(),
                                                    tr("Image Files (*.png *.jpg *.bmp)"),
                                                    &selfilter);

    if(!filename.isNull())
    {
        QImage matchImage;
        matchImage.load(filename);
        this->editor.histogramMatching(matchImage);
        this->showEditedImage();
    } else {
        printf("Error on on_actionLoadImage_triggered: filename is null");
    }
}

void MainWindow::on_rotateClockwiseButton_clicked()
{
    this->editor.RotateClockwise();
    this->showEditedImage();
}

void MainWindow::on_rotateCounterClockwiseButton_clicked()
{
    this->editor.RotateCounterClockwise();
    this->showEditedImage();
}

void MainWindow::on_convolutionButton_clicked()
{
    std::vector<std::vector<double>> kernel = {
        {this->ui->kernel00->value(), this->ui->kernel10->value(), this->ui->kernel20->value()},
        {this->ui->kernel01->value(), this->ui->kernel11->value(), this->ui->kernel21->value()},
        {this->ui->kernel02->value(), this->ui->kernel12->value(), this->ui->kernel22->value()}
    };
    bool sum127 = true;
    if (this->ui->kernelFilterDropdown->currentIndex() > 0 && this->ui->kernelFilterDropdown->currentIndex() < 4)
        sum127 = false;
    this->editor.Convolution3x3(kernel, sum127);
    this->showEditedImage();
}

void MainWindow::on_kernelFilterDropdown_currentIndexChanged(int index)
{
    qInfo() << "on_kernelFilterDropdown_currentIndexChanged:" << index;
    const std::vector<std::vector<double>> zero = {{0, 0, 0},
                                                   {0, 0, 0},
                                                   {0, 0, 0}};
    const std::vector<std::vector<double>> gaussian = {{0.0625, 0.125, 0.0625},
                                                       {0.1250, 0.250, 0.1250},
                                                       {0.0625, 0.125, 0.0625}};
    const std::vector<std::vector<double>> laplacian = {{0, -1, 0},
                                                       {-1, 4, -1},
                                                       {0, -1, 0}};
    const std::vector<std::vector<double>> highpass = {{-1, -1, -1},
                                                       {-1,  8, -1},
                                                       {-1, -1, -1}};
    const std::vector<std::vector<double>> prewitthx = {{-1, 0, 1},
                                                        {-1, 0, 1},
                                                        {-1, 0, 1}};
    const std::vector<std::vector<double>> prewitthy = {{-1, -1, -1},
                                                        {0, 0, 0},
                                                        {1, 1, 1}};
    const std::vector<std::vector<double>> sobelhx = {{-1, 0, 1},
                                                      {-2, 0, 2},
                                                      {-1, 0, 1}};
    const std::vector<std::vector<double>> sobelhy = {{-1, -2, -1},
                                                      { 0,  0, 0},
                                                      { 1, 2, 1}};

    std::vector<std::vector<std::vector<double>>> filters = {zero, gaussian, laplacian, highpass, prewitthx, prewitthy, sobelhx, sobelhy};

    this->ui->kernel00->setValue(filters[index][0][0]);
    this->ui->kernel10->setValue(filters[index][1][0]);
    this->ui->kernel20->setValue(filters[index][2][0]);
    this->ui->kernel01->setValue(filters[index][0][1]);
    this->ui->kernel11->setValue(filters[index][1][1]);
    this->ui->kernel21->setValue(filters[index][2][1]);
    this->ui->kernel02->setValue(filters[index][0][2]);
    this->ui->kernel12->setValue(filters[index][1][2]);
    this->ui->kernel22->setValue(filters[index][2][2]);

    if (index > 0) {
        this->ui->kernel00->setDisabled(true);
        this->ui->kernel10->setDisabled(true);
        this->ui->kernel20->setDisabled(true);
        this->ui->kernel01->setDisabled(true);
        this->ui->kernel11->setDisabled(true);
        this->ui->kernel21->setDisabled(true);
        this->ui->kernel02->setDisabled(true);
        this->ui->kernel12->setDisabled(true);
        this->ui->kernel22->setDisabled(true);
    } else {
        this->ui->kernel00->setDisabled(false);
        this->ui->kernel10->setDisabled(false);
        this->ui->kernel20->setDisabled(false);
        this->ui->kernel01->setDisabled(false);
        this->ui->kernel11->setDisabled(false);
        this->ui->kernel21->setDisabled(false);
        this->ui->kernel02->setDisabled(false);
        this->ui->kernel12->setDisabled(false);
        this->ui->kernel22->setDisabled(false);
    }
}

void MainWindow::on_zoomInButton_clicked()
{
    this->editor.ZoomIn();
    this->showEditedImage();
}

void MainWindow::on_infoButton_clicked()
{
    QString info = this->editor.Info();
    QMessageBox msgBox;
    msgBox.setText(info);
    msgBox.exec();
}

void MainWindow::on_zoomOutButton_clicked()
{
    int sx = this->ui->sxInput->value();
    int sy = this->ui->sxInput->value();
    this->editor.ZoomOut(sx, sy);
    this->showEditedImage();
}
