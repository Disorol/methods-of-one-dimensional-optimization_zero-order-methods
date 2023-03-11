#include <iostream>
#include <vector>
#include <math.h>

using namespace std;

class Optimization
{
protected:
    float const a = 0.25f; // Нижняя граница отрезка неопределённости
    float const b = 2.0f; // Верхняя граница отрезка неопределённости

    /*

        Целевая функция, заданная в условии.

        Вариант 29.

    */
    float f(float x) {
        return x * x + 1 / x;
    }

    /*

        Метод вывода ответов в консоль.

    */
    void responseOutput(float E, float _x, float _y, float E_guaranteed, int N, int N_estimated)
    {
        cout << E << " " << _x << " " << _y << " " << E_guaranteed << " " << N << " " << N_estimated << "\n";
    }

public:
    virtual void algorithm(int iteration) = 0;
};

/*

    Метод перебора.

    Сделан в соответствии с pdf (рукописной) документацией (на почте группы от 03.03.2023).

*/
class BruteForceMethod : Optimization
{
protected:
    float N_basic[5] = { 9, 99, 999, 9999, 99999 }; // Заданные значения N для метода перебора
    
public:
    void algorithm(int iteration) override
    {
        float E = pow(10, -iteration);
        float minimal_y = numeric_limits<float>::max();  
        float optimal_x, optimal_y, E_guaranteed;

        int N = N_basic[iteration - 1];

        vector <float> x(N);
        vector <float> y(N);

        for (int i = 1; i < N; i++)
        {
            x[i] = a + i * ((b - a) / (N + 1.0f));
            y[i] = f(x[i]);

            if (y[i] < minimal_y)
            {
                minimal_y = y[i];
                optimal_x = x[i];
            }
        }

        optimal_y = minimal_y;

        E_guaranteed = (b - a) / (N + 1);

        responseOutput(E, optimal_x, optimal_y, E_guaranteed, N, N);
    }
};

/*

    Пассивный оптимальный алгоритм.

    Сделан в соответствии с pdf (рукописной) документацией (на почте группы от 03.03.2023).

*/
class PassivOptimalAlgorithm : Optimization
{
protected:
    float N_basic[5] = { 9, 98, 998, 9998, 99998 }; // Заданные значения N для пассивного оптимального алгоритма
public:
    void algorithm(int iteration) override
    {
        float E = pow(10, -iteration);
        float delta = E / 100;
        float minimal_y = numeric_limits<float>::max();
        float optimal_x, optimal_y, E_guaranteed;

        int N = N_basic[iteration - 1];
        int l;

        if (N % 2 == 0)
        {
            vector <float> x(N + 2);
            vector <float> y(N + 2);

            x[0] = a;
            x[N + 1] = b;
            float k = N / 2;

            for (int i = 1; i < k; i++)
            {
                x[2 * i] = a + i * ((b - a) / (k + 1));
                x[2 * i - 1] = x[2 * i] - delta;
            }

            for (int i = 1; i < N; i++)
            {
                y[i] = f(x[i]);
            }

            for (int i = 1; i < N; i++)
            {
                if (y[i] < minimal_y)
                {
                    minimal_y = y[i];
                    l = i;
                }
            }

            optimal_x = (x[l - 1] + x[l + 1]) / 2;
            optimal_y = f(optimal_x);
            E_guaranteed = (x[l + 1] - x[l - 1]) / 2;
        }
        else
        {
            vector <float> x(N);
            vector <float> y(N);

            for (int i = 1; i < N; i++)
            {
                x[i] = a + i * ((b - a) / (N + 1));
                y[i] = f(x[i]);

                if (y[i - 1] < minimal_y)
                {
                    minimal_y = y[i];
                    optimal_x = x[i];
                }
            }

            optimal_y = minimal_y;
            E_guaranteed = (b - a) / (N + 1);
        }

        responseOutput(E, optimal_x, optimal_y, E_guaranteed, N, N);
    }
};

/*

    Метод поразрядного поиска.

    Сделан в соответствии с pdf (рукописной) документацией от (на почте группы 03.03.2023).

*/
class BitwiseSearchMethod : Optimization
{
public:
    void algorithm(int iteration) override
    {
        float E = pow(10, -iteration);
        float h = (b - a) / 4;
        float x0, x1, y0, y1;

        int N = 1;

        x0 = a;
        y0 = f(x0);

        do
        {
            x1 = x0 + h;
            y1 = f(x1);
            N++;

            if (y0 > y1)
            {
                x0 = x1;
                y0 = y1;

                if ((a < x0) && (x0 < b))
                {
                    continue;
                }
            }

            if (abs(h) <= E)
            {
                break;
            }
            else
            {
                x0 = x1;
                y0 = y1;
                h = -h / 4;
                continue;
            }

        } while (true);

        responseOutput(E, x0, y0, h, N, N); // Е гарантированное = h
    }
};

/*

    Метод деления отрезка пополам.

    Сделан в соответствии с docx (нерукописной) документацией (на почте группы от 27.02.2023).

*/
class DivisionMethod : Optimization
{
public:
    void algorithm(int iteration) override
    {
        float E = pow(10, -iteration);
        float a_changeable = a, b_changeable = b;
        float E_guaranteed, l;


        vector <float> x(4);
        vector <float> y(4);

        x[1] = (a_changeable + b_changeable) / 2;
        y[1] = f(x[1]);

        int N = 1;

        do
        {
            x[0] = (a_changeable + x[1]) / 2;
            y[0] = f(x[0]);
            N++;

            if (y[0] <= y[1])
            {
                b_changeable = x[1];
                x[1] = x[0];
                y[1] = y[0];
            }
            else
            {
                x[2] = (b_changeable + x[1]) / 2;
                y[2] = f(x[2]);
                N++;

                if (y[1] <= y[2])
                {
                    a_changeable = x[0];
                    b_changeable = x[2];
                }
                else
                {
                    a_changeable = x[1];
                    x[1] = x[2];
                    y[1] = y[2];
                }
            }

        } while (b_changeable - a_changeable > 2 * E);

        E_guaranteed = (b_changeable - a_changeable) / 2;

        responseOutput(E, x[1], y[1], E_guaranteed, N, N);
    }
};

/*

    Метод дихотомии.

    Сделан в соответствии с pdf (рукописной) документацией (на почте группы от 27.02.2023).

*/
class DichotomyMethod : Optimization
{
public:
    void algorithm(int iteration) override
    {
        float E = pow(10, -iteration);
        float delta = E / 100;
        float a_changeable = a, b_changeable = b;
        float x, optimal_x, y1, y2, optimal_y, E_guaranteed;

        int N = 0;

        do
        {
            x = (a_changeable + b_changeable) / 2;
            y1 = f(x - delta);
            y2 = f(x + delta);
            N += 2;

            if (y1 < y2)
            {
                b_changeable = x + delta;
            }
            else
            {
                a_changeable = x - delta;
            }

        } while (b_changeable - a_changeable > 2 * E);

        optimal_x = (a_changeable + b_changeable) / 2;
        optimal_y = f(x);
        E_guaranteed = (b_changeable - a_changeable) / 2;

        responseOutput(E, optimal_x, optimal_y, E_guaranteed, N, N);
    }
};

/*

    Метод золотого сечения.

    Сделан в соответствии с pdf (рукописной) документацией (на почте группы от 27.02.2023).

*/
class GoldenRatioMethod : Optimization
{
public:
    void algorithm(int iteration) override
    {
        float E = pow(10, -iteration);
        float r = (1 + sqrt(5)) / 2;
        float a_changeable = a, b_changeable = b;
        float x1, x2, optimal_x, y1, y2, optimal_y, E_guaranteed;

        x1 = b_changeable - (b_changeable - a_changeable) / r;
        y1 = f(x1);

        x2 = a_changeable + (b_changeable - a_changeable) / r;
        y2 = f(x2);

        int N = 2;

        do
        {
            if (y1 < y2)
            {
                b_changeable = x2;
                x2 = x1;
                y2 = y1;
                x1 = a_changeable + b_changeable - x2;
                y1 = f(x1);
            }
            else
            {
                a_changeable = x1;
                x1 = x2;
                y1 = y2;
                x2 = a_changeable + b_changeable - x1;
                y2 = f(x2);
            }

            N++;

        } while (b_changeable - a_changeable > 2 * E * r);

        if (y1 < y2)
        {
            b_changeable = x2;
        }
        else
        {
            a_changeable = x1;
        }

        optimal_x = (a_changeable + b_changeable) / 2;
        optimal_y = f(optimal_x);
        E_guaranteed = (b_changeable - a_changeable) / 2;

        responseOutput(E, optimal_x, optimal_y, E_guaranteed, N, N);
    }
};

int main()
{
    setlocale(LC_ALL, "Russian");

    cout << "Метод перебора:\n\n";

    BruteForceMethod* bruteForceMethod = new BruteForceMethod();
    
    for (int i = 1; i <= 5; i++)
        bruteForceMethod->algorithm(i);

    bruteForceMethod->~BruteForceMethod();

    delete bruteForceMethod;

    cout << "\nПассивный оптимальный алгоритм:\n\n";

    PassivOptimalAlgorithm* passivOptimalAlgorithm = new PassivOptimalAlgorithm();

    for (int i = 1; i <= 5; i++)
        passivOptimalAlgorithm->algorithm(i);

    passivOptimalAlgorithm->~PassivOptimalAlgorithm();

    delete passivOptimalAlgorithm;

    cout << "\nМетод поразрядного поиска:\n\n";

    BitwiseSearchMethod* bitwiseSearchMethod = new BitwiseSearchMethod();

    for (int i = 1; i <= 5; i++)
        bitwiseSearchMethod->algorithm(i);

    bitwiseSearchMethod->~BitwiseSearchMethod();

    delete bitwiseSearchMethod;

    cout << "\nМетод деления отрезка пополам:\n\n";

    DivisionMethod* divisionMethod = new DivisionMethod();

    for (int i = 1; i <= 5; i++)
        divisionMethod->algorithm(i);

    divisionMethod->~DivisionMethod();

    delete divisionMethod;

    cout << "\nМетод дихотомии:\n\n";

    DichotomyMethod* dichotomyMethod = new DichotomyMethod();

    for (int i = 1; i <= 5; i++)
        dichotomyMethod->algorithm(i);

    dichotomyMethod->~DichotomyMethod();

    delete dichotomyMethod;

    cout << "\nМетод золотого сечения:\n\n";

    GoldenRatioMethod* goldenRatioMethod = new GoldenRatioMethod();

    for (int i = 1; i <= 5; i++)
        goldenRatioMethod->algorithm(i);

    goldenRatioMethod->~GoldenRatioMethod();

    delete goldenRatioMethod;
}