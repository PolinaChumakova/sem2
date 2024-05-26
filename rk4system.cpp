#include <iostream>
#include <cmath>
#include <vector>

// Структура для хранения состояний системы
struct State {
    double u1;
    double u2;
};

// Параметры задачи
struct Params {
    double a1, a2, b1, b2, c1, c2, d1, d2;
};

// Функции для системы дифференциальных уравнений
State system(const State& state, double t) { //создаем структуру, в которой будем хранить состояния системы
    State derivatives;
    derivatives.u1 = 9 * state.u1 + 24 * state.u2 + 5 * cos(t) - (1.0 / 3) * sin(t);
    derivatives.u2 = -24 * state.u1 - 51 * state.u2 + 9 * cos(t) + (1.0 / 3) * sin(t);
    return derivatives;
}

// Метод Рунге-Кутты 4-го порядка
void rungeKutta4(State& state, double t, double h, int steps, std::vector<double>& t_values, std::vector<State>& states) {
    for (int i = 0; i < steps; ++i) {
        State k1 = system(state, t);
        State k2 = system({state.u1 + h / 2 * k1.u1, state.u2 + h / 2 * k1.u2}, t + h / 2);
        State k3 = system({state.u1 + h / 2 * k2.u1, state.u2 + h / 2 * k2.u2}, t + h / 2);
        State k4 = system({state.u1 + h * k3.u1, state.u2 + h * k3.u2}, t + h);

        state.u1 += h / 6 * (k1.u1 + 2 * k2.u1 + 2 * k3.u1 + k4.u1);
        state.u2 += h / 6 * (k1.u2 + 2 * k2.u2 + 2 * k3.u2 + k4.u2);
        t += h;

        t_values.push_back(t);
        states.push_back(state);
    }
}

// Аналитическое решение
State analyticalSolution(double t) {
    State result;
    result.u1 = 2 * exp(-3 * t) - exp(-39 * t) + (1.0 / 3) * cos(t);
    result.u2 = -exp(-3 * t) + 2 * exp(-39 * t) - (1.0 / 3) * cos(t);
    return result;
}

int main() {
    double h = 0.01;  // Шаг интегрирования
    int max_steps = 1000;  // Максимальное количество шагов
    double tolerance = 0.001;  // Допустимая погрешность

    // Начальные условия
    State state = {4.0 / 3.0, 2.0 / 3.0};
    double t0 = 0.0;

    std::vector<double> t_values;
    std::vector<State> states;

    rungeKutta4(state, t0, h, max_steps, t_values, states);

    // Вывод результатов и сравнение с аналитическим решением
    std::cout << "t\tNumerical u1\tAnalytical u1\tDifference u1\tNumerical u2\tAnalytical u2\tDifference u2\n";
    double max_difference_u1 = 0.0;
    double max_difference_u2 = 0.0;
    for (size_t i = 0; i < t_values.size(); ++i) {
        double t = t_values[i];
        State numerical = states[i];
        State analytical = analyticalSolution(t);
        double difference_u1 = std::abs(numerical.u1 - analytical.u1);
        double difference_u2 = std::abs(numerical.u2 - analytical.u2);

        if (difference_u1 > max_difference_u1) {
            max_difference_u1 = difference_u1;
        }
        if (difference_u2 > max_difference_u2) {
            max_difference_u2 = difference_u2;
        }

        std::cout << t << "\t" << numerical.u1 << "\t" << analytical.u1 << "\t" << difference_u1
                  << "\t" << numerical.u2 << "\t" << analytical.u2 << "\t" << difference_u2 << "\n";
    }

    std::cout << "Maximum difference u1: " << max_difference_u1 << "\n";
    std::cout << "Maximum difference u2: " << max_difference_u2 << "\n";

    // Поиск минимального шага
    double min_h = h;
    int iteration = 0;
    while ((max_difference_u1 > tolerance || max_difference_u2 > tolerance) && min_h > 1e-6 && iteration < max_steps) {
        min_h /= 2;
        t_values.clear();
        states.clear();
        state = {4.0 / 3.0, 2.0 / 3.0};  // Сброс начальных условий
        rungeKutta4(state, t0, min_h, max_steps, t_values, states);

        max_difference_u1 = 0.0;
        max_difference_u2 = 0.0;
        for (size_t i = 0; i < t_values.size(); ++i) {
            double t = t_values[i];
            State numerical = states[i];
            State analytical = analyticalSolution(t);
            double difference_u1 = std::abs(numerical.u1 - analytical.u1);
            double difference_u2 = std::abs(numerical.u2 - analytical.u2);

            if (difference_u1 > max_difference_u1) {
                max_difference_u1 = difference_u1;
            }
            if (difference_u2 > max_difference_u2) {
                max_difference_u2 = difference_u2;
            }
        }
        iteration++;
    }

    std::cout << "Minimum step size where difference <= 0.001: " << min_h << "\n";

    return 0;
}