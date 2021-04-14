#ifndef PHOTOMANAGER_H
#define PHOTOMANAGER_H

#include <QString>
#include <QImage>

class PhotoManager
{
private:
    QImage originalImage;
    QImage editedImage;

public:
    PhotoManager();

    QImage getOriginalImage();
    QImage getEditedImage();
    void setEditedImage(QImage image);

    QString Info();

    void revertImageChanges();

    void loadImage(QString filename);
    void saveImage(QString filename);

    void flipVertically();
    void flipHorizontally();

    QImage toGrayscale(QImage image);
    void quantize(int maxTones);

    std::vector<int> generateGrayscaleHistogram(QImage image);
    void adjustBrightness(int brightLevel);
    void adjustContrast(double contrastFactor);
    void toNegative();

    std::vector<int> histogram();
    void histogramEqualization();
    void histogramMatching(QImage matchImage);
    std::vector<int> generateCumulativeHistogram(int width, int height, std::vector<int> histogram);

    void RotateClockwise();
    void RotateCounterClockwise();
    void ZoomIn();
    void ZoomOut(int sx, int sy);

    void Convolution3x3(std::vector<std::vector<double>> kernel, bool sum127);
};

#endif // PHOTOMANAGER_H
