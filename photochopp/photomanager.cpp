#include "photomanager.h"
#include <algorithm>
#include<QDebug>

PhotoManager::PhotoManager()
{

}

QImage PhotoManager::getOriginalImage() {
    return this->originalImage;
}

QImage PhotoManager::getEditedImage() {
    return this->editedImage;
}

void PhotoManager::setEditedImage(QImage image) {
    this->editedImage = image;
}

QString PhotoManager::Info() {
    QString info = "Width: ";
    info.append(QString::number(this->editedImage.width()));
    info.append(" | Height: ");
    info.append(QString::number(this->editedImage.height()));

    return info;
}

void PhotoManager::loadImage(QString filename)
{
    qInfo() << "loadImage";
    this->originalImage.load(filename);
    this->editedImage = this->originalImage.copy();
}

void PhotoManager::saveImage(QString filename)
{
    qInfo() << "saveImage";
    this->editedImage.save(filename);
}

void PhotoManager::flipVertically()
{
    int height = this->editedImage.height();
    int width = this->editedImage.width();
    qInfo() << "flipVertically";

    for(int row = 0; row < height/2; ++row) {
        for(int col = 0; col < width; ++col) {
            QRgb px1 = this->editedImage.pixel(col, row);
            QRgb px2 = this->editedImage.pixel(col, (height-row-1));

            this->editedImage.setPixel(col, (height-row-1), px1);
            this->editedImage.setPixel(col, row, px2);
        }
    }
}

void PhotoManager::flipHorizontally(){
    int height = this->editedImage.height();
    int width = this->editedImage.width();
    qInfo() << "flipHorizontally";

    for(int row = 0; row < height; ++row) {
        for(int col = 0; col < width/2; ++col) {
            QRgb px1 = this->editedImage.pixel(col, row);
            QRgb px2 = this->editedImage.pixel((width-col-1), row);

            this->editedImage.setPixel(col, row, px2);
            this->editedImage.setPixel((width-col-1), row, px1);
        }
    }
}

QImage PhotoManager::toGrayscale(QImage image){
    qInfo() << "toGrayscale";
    int height = image.height();
    int width = image.width();

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            QColor px(image.pixel(x, y));
            int luminance = (0.299*px.red() + 0.587*px.green() + 0.114*px.blue());
            image.setPixelColor(x, y, QColor(luminance, luminance, luminance));
        }
    }

    return image;
}

void PhotoManager::revertImageChanges(){
    this->editedImage = this->originalImage.copy();
}

void PhotoManager::quantize(int maxTones) {
    qInfo() << "quantinze: tones:" << maxTones;

    int binSize = 255 / (maxTones-1);

    for (int y = 0; y < this->editedImage.height(); ++y)
        for (int x = 0; x < this->editedImage.width(); ++x) {

            QColor px(this->editedImage.pixel(x, y));
            int luminance = (0.299*px.red() + 0.587*px.green() + 0.114*px.blue());
            luminance = static_cast<int>(std::round(luminance / binSize) * binSize);

            this->editedImage.setPixelColor(x, y, QColor(luminance, luminance, luminance));
        }
}

std::vector<int> PhotoManager::generateGrayscaleHistogram(QImage image)
{
    std::vector<int> histogram(256);
    int width = image.width();
    int height = image.height();

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; ++x) {
            QColor px(image.pixel(x, y));
            histogram[px.red()]++;
        }
    }

    return histogram;
}

void PhotoManager::adjustBrightness(int brightLevel){
    qInfo() << "adjustBrightness: brightLevel=" << brightLevel;
    int width = this->editedImage.width();
    int height = this->editedImage.height();

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; ++x) {
            QColor px(this->editedImage.pixel(x, y));
            int red = qMax(qMin(px.red()+brightLevel, 255), 0);
            int green = qMax(qMin(px.green()+brightLevel, 255), 0);
            int blue = qMax(qMin(px.blue()+brightLevel, 255), 0);

            this->editedImage.setPixelColor(x, y, QColor(red, green, blue));
        }
    }
}

void PhotoManager::adjustContrast(double contrastFactor){
    qInfo() << "adjustContrast: constrastFactor=" << contrastFactor;
    int width = this->editedImage.width();
    int height = this->editedImage.height();

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; ++x) {
            QColor px(this->editedImage.pixel(x, y));
            int red = qMin(qRound(px.red()*contrastFactor), 255);
            int green = qMin(qRound(px.green()*contrastFactor), 255);
            int blue = qMin(qRound(px.blue()*contrastFactor), 255);

            this->editedImage.setPixelColor(x, y, QColor(red, green, blue));
        }
    }
}

void PhotoManager::toNegative(){
    qInfo() << "toNegative";
    int width = this->editedImage.width();
    int height = this->editedImage.height();

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; ++x) {
            QColor px(this->editedImage.pixel(x, y));
            int red = 255 - px.red();
            int green = 255 - px.green();
            int blue = 255 - px.blue();

            this->editedImage.setPixelColor(x, y, QColor(red, green, blue));
        }
    }
}

std::vector<int> PhotoManager::generateCumulativeHistogram(int width, int height, std::vector<int> histogram){
    float alpha = 255.0/(width*height);
    std::vector<int> cumulativeHistogram(256);

    for (int i = 1; i < 256; i++){
        cumulativeHistogram[i] = cumulativeHistogram[i-1] + alpha * histogram[i];
        if (cumulativeHistogram[i] > 255)
            cumulativeHistogram[i] = 255;
    }
    return cumulativeHistogram;
};

void PhotoManager::histogramEqualization(){
    qInfo() << "histogramEqualization";

    int width = this->editedImage.width();
    int height = this->editedImage.height();
    auto histogram = this->generateGrayscaleHistogram(this->editedImage);

    std::vector<int> cumulativeHistogram(256);
    cumulativeHistogram = generateCumulativeHistogram(width, height, histogram);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; ++x) {
            QColor px(this->editedImage.pixel(x, y));
            int r = cumulativeHistogram[px.red()];
            int g = cumulativeHistogram[px.green()];
            int b = cumulativeHistogram[px.blue()];

            this->editedImage.setPixelColor(x, y, QColor(r, g, b));
        }
    }
}

void PhotoManager::histogramMatching(QImage matchImage) {
    qInfo() << "histogramMatching";
    auto histogram = this->generateGrayscaleHistogram(this->editedImage);
    auto histogramMatch = this->generateGrayscaleHistogram(matchImage);

    int width = this->editedImage.width();
    int height = this->editedImage.height();
    auto cumulativeHistogram = generateCumulativeHistogram(width, height, histogram);
    auto matchCumulativeHistogram = generateCumulativeHistogram(matchImage.width(), matchImage.height(), histogramMatch);

    // for each source shade find the closest target shade with most similar cumulative value
    std::vector<int> HM(256);
    for (int i = 0; i < 256; i++) {
        int closestShadeDiff = 256;
        int closestShadeIndex = 0;
        int shade = cumulativeHistogram[i];

        for (int j = 0; j < 256; j++) {
            int matchShade = matchCumulativeHistogram[j];
            int shadeDiff = abs(matchShade - shade);

            if (shadeDiff < closestShadeDiff) {
                closestShadeDiff = shadeDiff;
                closestShadeIndex = j;
            }
            if (closestShadeDiff == 0) break;
        }
        HM[i] = closestShadeIndex;
    }

    // perform histogram matching
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; ++x) {
            QColor px(this->editedImage.pixel(x, y));
            int r = HM[px.red()];
            int g = HM[px.green()];
            int b = HM[px.blue()];
            this->editedImage.setPixelColor(x, y, QColor(r, g, b));
        }
}

void PhotoManager::RotateClockwise(){
    qInfo() << "RotateClockwise";
    int width = this->editedImage.width();
    int height = this->editedImage.height();
    QImage rotatedImg(height, width, QImage::Format_RGB32);

    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; ++x) {
            QColor px(this->editedImage.pixel(x, y));
            rotatedImg.setPixelColor(height-1-y, x, px);
        }
    this->editedImage = rotatedImg;
}

void PhotoManager::RotateCounterClockwise(){
    qInfo() << "RotateCounterClockwise";
    int width = this->editedImage.width();
    int height = this->editedImage.height();
    QImage rotatedImg(height, width, QImage::Format_RGB32);

    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; ++x) {
            QColor px(this->editedImage.pixel(x, y));
            rotatedImg.setPixelColor(y, (width-1)-x, px);
        }
    this->editedImage = rotatedImg;
}

void PhotoManager::Convolution3x3(std::vector<std::vector<double>> kernel, bool sum127 = true){
    qInfo() << "Convolution3x3";

    int width = this->editedImage.width();
    int height = this->editedImage.height();
    QImage resultImage(width, height, QImage::Format_RGB32);
    resultImage.fill(Qt::black);

    for (int y = 1; y < height-1; y++)
        for (int x = 1; x < width-1; x++) {

            int newPxColor = 0;
            for (int yk = -1; yk <= 1; yk++)
                for (int xk = -1; xk <= 1; xk++){
                    QRgb px(this->editedImage.pixel(x+xk, y+yk));
                    newPxColor += round((kernel[xk+1][yk+1] * qRed(px)));
                }
            if (sum127) newPxColor += 127;
            newPxColor = fmin(fmax(newPxColor, 0), 255);
            resultImage.setPixelColor(x, y, QColor(newPxColor, newPxColor, newPxColor));
        }
    this->editedImage = resultImage;
}

void PhotoManager::ZoomOut(int sx, int sy){
    int width = this->editedImage.width();
    int height = this->editedImage.height();

    int resultWidth = ceil(width/sx);
    int resultHeight = ceil(height/sy);
    QImage resultImage(resultWidth, resultHeight, QImage::Format_RGB32);

    int origX = 0;
    for (int x = 0; x < resultWidth; x++){
        int origY = 0;
        for (int y = 0; y < resultHeight; y++){

            int cumRed = 0;
            int cumGreen = 0;
            int cumBlue = 0;
            int squareCount = 0;

            for (int sqX = 0; sqX < sx; sqX++){
                for (int sqY = 0; sqY < sy; sqY++){
                    if(x+sqX < width && y+sqY < height){
                        QRgb px = this->editedImage.pixel(origX+sqX, origY+sqY);
                        cumRed += qRed(px);
                        cumGreen += qGreen(px);
                        cumBlue += qBlue(px);
                        squareCount++;
                    }
                }
            }

            QRgb newPx = qRgb(cumRed/squareCount, cumGreen/squareCount, cumBlue/squareCount);
            resultImage.setPixelColor(x, y, newPx);
            origY+=sy;
        }
        origX+=sx;
    }

    this->editedImage = resultImage;
}

void PhotoManager::ZoomIn(){
    int width = this->editedImage.width();
    int height = this->editedImage.height();

    int zoomedWidth = (2 * width) - 1;
    int zoomedHeight = (2 * height) - 1;

    QImage targetImage(zoomedWidth, zoomedHeight, QImage::Format_RGB32);

    auto average = [](QRgb* a, QRgb* b){
        auto red = (qRed(*a) + qRed(*b)) / 2;
        auto green = (qGreen(*a) + qGreen(*b)) / 2;
        auto blue = (qBlue(*a) + qBlue(*b)) / 2;
        return qRgb(red, green, blue);
    };

    // Columns
    int newX;
    int newY = 0;
    for (int y = 0; y < height; y++, newY+=2) {
        newX = 0;
        for (int x = 0; x < width; x++, newX+=2) {
            QRgb px(this->editedImage.pixel(x, y));
            targetImage.setPixelColor(newX, newY, px);
            if (x+1 < width && newX+1 < zoomedWidth) {
                QRgb pxNext(this->editedImage.pixel(x+1, y));
                targetImage.setPixelColor(newX+1, newY, average(&px, &pxNext));
            }
        }
    }

    // Lines
    for (int y = 1; y < zoomedHeight; y+=2)
        for (int x = 0; x < zoomedWidth; x++) {
            QRgb px(targetImage.pixel(x, y-1));
            QRgb pxNext(targetImage.pixel(x, y+1));
            targetImage.setPixelColor(x, y, average(&px, &pxNext));
        }

    this->editedImage = targetImage;
}


