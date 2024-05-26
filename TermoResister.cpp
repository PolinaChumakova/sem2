#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>

int main() {
    // Пример данных
    std::vector<double> adcReadings = {102, 98, 105, 97, 100, 300}; // данные АЦП (возможно, с выбросами)
    std::vector<double> temperatures = {25, 24, 26, 23, 25, 80};    // реальные температуры (возможно, с выбросами)

    // Преобразуем данные в подходящий для OpenCV формат
    cv::Mat adcMat(adcReadings.size(), 1, CV_64F, adcReadings.data());
    cv::Mat tempMat(temperatures.size(), 1, CV_64F, temperatures.data());

    // Установливаем параметры RANSAC
    int maxIters = 1000;         // Максимальное количество итераций
    double inlierThreshold = 2;  // Порог для определения выбросов

    // Выполняем линейную регрессию с использованием RANSAC
    cv::Vec4f lineParams;
    cv::fitLine(tempMat, adcMat, lineParams, cv::DIST_L2, 0, 0.01, 0.01);

    // Извлекаем параметры линии
    double slope = lineParams[1] / lineParams[0];
    double intercept = lineParams[3] - slope * lineParams[2];

    std::cout << "Slope: " << slope << std::endl;
    std::cout << "Intercept: " << intercept << std::endl;

    // Построение модели с RANSAC
    std::vector<uchar> inliers;
    cv::estimateAffinePartial2D(tempMat, adcMat, inliers, cv::RANSAC, inlierThreshold, maxIters);

    // Вывод результатов RANSAC
    for (size_t i = 0; i < inliers.size(); i++) {
        if (inliers[i]) {
            std::cout << "Inlier: " << adcReadings[i] << " -> " << temperatures[i] << std::endl;
        } else {
            std::cout << "Outlier: " << adcReadings[i] << " -> " << temperatures[i] << std::endl;
        }
    }

    // Визуализация данных и линии регрессии
    int width = 800;
    int height = 600;
    cv::Mat plot = cv::Mat::zeros(height, width, CV_8UC3);
    
    // Настройки масштабирования для удобства отображения
    double scaleX = width / (*std::max_element(temperatures.begin(), temperatures.end()) - *std::min_element(temperatures.begin(), temperatures.end()));
    double scaleY = height / (*std::max_element(adcReadings.begin(), adcReadings.end()) - *std::min_element(adcReadings.begin(), adcReadings.end()));

    // Отображение данных
    for (size_t i = 0; i < adcReadings.size(); i++) {
        int x = (temperatures[i] - *std::min_element(temperatures.begin(), temperatures.end())) * scaleX;
        int y = height - (adcReadings[i] - *std::min_element(adcReadings.begin(), adcReadings.end())) * scaleY;
        cv::circle(plot, cv::Point(x, y), 5, inliers[i] ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255), -1);
    }

    // Отображение линии регрессии
    double x1 = *std::min_element(temperatures.begin(), temperatures.end());
    double y1 = slope * x1 + intercept;
    double x2 = *std::max_element(temperatures.begin(), temperatures.end());
    double y2 = slope * x2 + intercept;

    cv::line(plot, cv::Point((x1 - *std::min_element(temperatures.begin(), temperatures.end())) * scaleX, height - (y1 - *std::min_element(adcReadings.begin(), adcReadings.end())) * scaleY),
             cv::Point((x2 - *std::min_element(temperatures.begin(), temperatures.end())) * scaleX, height - (y2 - *std::min_element(adcReadings.begin(), adcReadings.end())) * scaleY), cv::Scalar(255, 255, 255), 2);

    // Отображение графика
    cv::imshow("RANSAC Regression", plot);
    cv::waitKey(0);

    return 0;
}