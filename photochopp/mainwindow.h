#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "photomanager.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showOriginalImage();

    void showEditedImage();

    void on_actionLoadImage_triggered();

    void on_actionSaveImage_triggered();

    void on_verticalFlipButton_clicked();

    void on_horizontalFilpButton_clicked();

    void on_grayscaleButton_clicked();

    void on_revertButton_clicked();

    void on_quantize_clicked();

    void on_loadImageButton_clicked();

    void showHistogram(std::vector<int> histogram);

    void on_histogramButton_clicked();

    void on_brightnessButton_clicked();

    void on_contrastButton_clicked();

    void on_negativeButton_clicked();

    void on_histogramEqualizationButton_clicked();

    void on_histogramMatchingButton_clicked();

    void on_rotateClockwiseButton_clicked();

    void on_rotateCounterClockwiseButton_clicked();

    void on_convolutionButton_clicked();

    void on_kernelFilterDropdown_currentIndexChanged(int index);

    void on_zoomInButton_clicked();

    void on_infoButton_clicked();

    void on_zoomOutButton_clicked();

private:
    PhotoManager editor;

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
