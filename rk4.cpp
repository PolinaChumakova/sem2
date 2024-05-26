#include <iostream>
#include <cmath>
#include <vector>

// Параметры задачи
struct Params {
    double a;
    double b;
    double d;
};

// Функция для дифференциального уравнения dy/dt = at - by
double func(double t, double y, const Params& params) {
    return params.a * t - params.b * y;
}

// Метод Рунге-Кутты 4-го порядка
void rungeKutta4(double y0, double t0, double h, int steps, const Params& params, std::vector<double>& t_values, std::vector<double>& y_values) {
    double y = y0;
    double t = t0;

    for (int i = 0; i < steps; ++i) {
        double k1 = h * func(t, y, params);
        double k2 = h * func(t + h / 2, y + k1 / 2, params);
        double k3 = h * func(t + h / 2, y + k2 / 2, params);
        double k4 = h * func(t + h, y + k3, params);

        y += (k1 + 2 * k2 + 2 * k3 + k4) / 6;
        t += h;

        t_values.push_back(t); //записываем в векторный массив шаги по времени и значения целевой переменной
        y_values.push_back(y);
    }
}

// Аналитическое решение было данно в условии
double analyticalSolution(double t, const Params& params, double C) {
    return params.a / (params.b * params.b) * (t - 1 / params.b) + C * exp(-params.b * t);
}

int main() {
    Params params = {-0.7, 1.3, 0.5};
    double C = params.d + params.a / (params.b * params.b);
    double y0 = params.d;
    double t0 = 0.0;
    double h = 0.1;
    double tolerance = 0.001;
    int max_steps = 1000;

    std::vector<double> t_values;
    std::vector<double> y_values;

    rungeKutta4(y0, t0, h, max_steps, params, t_values, y_values);

    // Вывод результатов и сравнение с аналитическим решением
    std::cout << "t\tNumerical\tAnalytical\tDifference\n";
    double max_difference = 0.0;
    for (size_t i = 0; i < t_values.size(); ++i) {
        double t = t_values[i];
        double numerical = y_values[i];
        double analytical = analyticalSolution(t, params, C);
        double difference = std::abs(numerical - analytical); //считаем разницу между аналитическим методом и рк4

        if (difference > max_difference) {  
            max_difference = difference;   // Сохраняем наибольшую разницу
        }

        std::cout << t << "\t" << numerical << "\t" << analytical << "\t" << difference << "\n";
    }

    std::cout << "Maximum difference: " << max_difference << "\n";

    // Поиск минимального шага
    double min_h = h;
    while (max_difference > tolerance && min_h > 1e-6) { //пока максимальная разница не станет равна 0.001 будет искать минимальный шаг, 
        min_h /= 2;                                      // сокращая его в два раза с каждой итерацией
        t_values.clear();
        y_values.clear();
        rungeKutta4(y0, t0, min_h, max_steps, params, t_values, y_values);

        max_difference = 0.0;
        for (size_t i = 0; i < t_values.size(); ++i) {
            double t = t_values[i];
            double numerical = y_values[i];
            double analytical = analyticalSolution(t, params, C);
            double difference = std::abs(numerical - analytical);

            if (difference > max_difference) {
                max_difference = difference;
            }
        }
    }

    std::cout << "Minimum step size where difference <= 0.001: " << min_h << "\n";

    return 0;
}