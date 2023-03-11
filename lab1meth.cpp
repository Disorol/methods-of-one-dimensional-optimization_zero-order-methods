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

class BruteForceMethod : Optimization
{
protected:
    float N_basic[5] = { 9, 99, 999, 9999, 99999 }; // Заданные значения N
    
public:
    void algorithm(int iteration) override
    {
        float min_y = numeric_limits<float>::max(),
            E = pow(10, -iteration),
            min_x,
            E_guaranteed;

        int N = N_basic[iteration - 1];

        vector <float> x_vector(N);
        vector <float> y_vector(N);

        for (int i = 1; i < N; i++)
        {
            x_vector[i] = a + i * ((b - a) / (N + 1.0f));
            y_vector[i] = f(x_vector[i]);

            if (y_vector[i] < min_y)
            {
                min_y = y_vector[i];
                min_x = x_vector[i];
            }
        }

        E_guaranteed = (b - a) / (N + 1);

        responseOutput(E, min_x, min_y, E_guaranteed, N, N);
    }
};

class PassivOptimalAlgorithm : Optimization
{
protected:
    float N_basic[5] = { 9, 98, 998, 9998, 99998 }; // Заданные значения N
public:
    void algorithm(int iteration) override
    {
        float E = pow(10, -iteration),
            _x,
            min_y = numeric_limits<float>::max(),
            _y,
            delta = E / 100, E_guaranteed;

        int N = N_basic[iteration - 1],
            l;

        if (N % 2 == 0)
        {
            vector <float> x_vector(N + 2);
            vector <float> y_vector(N + 2);

            x_vector[0] = a;
            x_vector[N + 1] = b;
            float k = N / 2;

            for (int i = 1; i < k; i++)
            {
                x_vector[2 * i] = a + i * ((b - a) / (k + 1));
                x_vector[2 * i - 1] = x_vector[2 * i] - delta;
            }

            for (int i = 1; i < N; i++)
            {
                y_vector[i] = f(x_vector[i]);
            }

            for (int i = 1; i < N; i++)
            {
                if (y_vector[i] < min_y)
                {
                    min_y = y_vector[i];
                    l = i;
                }
            }

            _x = (x_vector[l - 1] + x_vector[l + 1]) / 2;
            _y = f(_x);
            E_guaranteed = (x_vector[l + 1] - x_vector[l - 1]) / 2;
        }
        else
        {
            vector <float> x_vector(N);
            vector <float> y_vector(N);
            _y = numeric_limits<float>::max();

            for (int i = 1; i < N; i++)
            {
                x_vector[i] = a + i * ((b - a) / (N + 1));
                y_vector[i] = f(x_vector[i]);

                if (y_vector[i - 1] < _y)
                {
                    _y = y_vector[i];
                    _x = x_vector[i];
                }
            }

            E_guaranteed = (b - a) / (N + 1);
        }

        responseOutput(E, _x, _y, E_guaranteed, N, N);
    }
};

class BitwiseSearchMethod : Optimization
{
public:
    void algorithm(int iteration) override
    {
        float h = (b - a) / 4,
            E = pow(10, -iteration),
            x1, x2, y1, y2;

        int N = 1;

        x1 = a;
        y1 = f(x1);

        do
        {
            x2 = x1 + h;
            y2 = f(x2);
            N++;

            if (y1 > y2)
            {
                x1 = x2;
                y1 = y2;

                if ((a < x1) && (x1 < b))
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
                x1 = x2;
                y1 = y2;
                h = -h / 4;
                continue;
            }
        } while (true);

        responseOutput(E, x1, y1, h, N, N); // Е гарантированное = h
    }
};

class DivisionMethod : Optimization
{
public:
    void algorithm(int iteration) override
    {
        float E = pow(10, -iteration),
            a_changeable = a,
            b_changeable = b,
            Eg;

        vector <float> x_vector(4);
        vector <float> y_vector(4);

        int N = 1;

        x_vector[1] = (a_changeable + b_changeable) / 2;
        y_vector[1] = f(x_vector[1]);

        do
        {
            x_vector[0] = (a_changeable + x_vector[1]) / 2;
            y_vector[0] = f(x_vector[0]);
            N++;

            if (y_vector[0] <= y_vector[1])
            {
                b_changeable = x_vector[1];
                x_vector[1] = x_vector[0];
                y_vector[1] = y_vector[0];
            }
            else
            {
                x_vector[2] = (b + x_vector[1]) / 2;
                y_vector[2] = f(x_vector[2]);
                N++;

                if (y_vector[1] <= y_vector[2])
                {
                    a_changeable = x_vector[0];
                    b_changeable = x_vector[2];
                }
                else
                {
                    a_changeable = x_vector[1];
                    x_vector[1] = x_vector[2];
                    y_vector[1] = y_vector[2];
                }
            }
        } while (b_changeable - a_changeable > 2 * E);

        Eg = (b_changeable - a_changeable) / 2;

        responseOutput(E, x_vector[1], y_vector[1], Eg, N, N);
    }
};

class DichotomyMethod : Optimization
{
public:
    void algorithm(int iteration) override
    {
        float E = pow(10, -iteration),
            delta = E / 100,
            a_changeable = a,
            b_changeable = b,
            x, _x, y1, y2, _y, E_guaranteed;

        int N = 0;

        do
        {
            x = (a_changeable + b_changeable) / 2;
            y1 = f(x - delta);
            y2 = f(x + delta);
            N = N + 2;

            if (y1 < y2)
            {
                b_changeable = x + delta;
            }
            else
            {
                a_changeable = x - delta;
            }
        } while (b_changeable - a_changeable > 2 * E);

        _x = (a_changeable + b_changeable) / 2;
        _y = f(x);
        E_guaranteed = (b_changeable - a_changeable) / 2;

        responseOutput(E, _x, _y, E_guaranteed, N, N);
    }
};

class GoldenRatioMethod : Optimization
{
public:
    void algorithm(int iteration) override
    {
        float E = pow(10, -iteration),
            r = (1 + sqrt(5)) / 2,
            a_changeable = a,
            b_changeable = b,
            x1, x2, _x, y1, y2, _y, E_guaranteed;

        int N = 2;

        x1 = b_changeable - (b_changeable - a_changeable) / r;
        x2 = a_changeable + (b_changeable - a_changeable) / r;
        y1 = f(x1);
        y2 = f(x2);

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

        } while (b_changeable - a_changeable > 2 / E * r);

        if (y1 < y2)
        {
            b_changeable = x2;
        }
        else
        {
            a_changeable = x1;
        }

        _x = (a_changeable + b_changeable) / 2;
        _y = f(_x);
        E_guaranteed = (b_changeable - a_changeable) / 2;

        responseOutput(E, _x, _y, E_guaranteed, N, N);
    }
};

int main()
{
    BruteForceMethod* bruteForceMethod = new BruteForceMethod();
    
    for (int i = 1; i <= 5; i++)
        bruteForceMethod->algorithm(i);

    cout << "\n";

    bruteForceMethod->~BruteForceMethod();

    delete bruteForceMethod;

    PassivOptimalAlgorithm* passivOptimalAlgorithm = new PassivOptimalAlgorithm();

    for (int i = 1; i <= 5; i++)
        passivOptimalAlgorithm->algorithm(i);

    cout << "\n";

    passivOptimalAlgorithm->~PassivOptimalAlgorithm();

    delete passivOptimalAlgorithm;

    BitwiseSearchMethod* bitwiseSearchMethod = new BitwiseSearchMethod();

    for (int i = 1; i <= 5; i++)
        bitwiseSearchMethod->algorithm(i);

    cout << "\n";

    bitwiseSearchMethod->~BitwiseSearchMethod();

    delete bitwiseSearchMethod;

    DivisionMethod* divisionMethod = new DivisionMethod();

    for (int i = 1; i <= 5; i++)
        divisionMethod->algorithm(i);

    cout << "\n";

    divisionMethod->~DivisionMethod();

    delete divisionMethod;

    DichotomyMethod* dichotomyMethod = new DichotomyMethod();

    for (int i = 1; i <= 5; i++)
        dichotomyMethod->algorithm(i);

    cout << "\n";

    dichotomyMethod->~DichotomyMethod();

    delete dichotomyMethod;

    GoldenRatioMethod* goldenRatioMethod = new GoldenRatioMethod();

    for (int i = 1; i <= 5; i++)
        goldenRatioMethod->algorithm(i);

    cout << "\n";

    goldenRatioMethod->~GoldenRatioMethod();

    delete goldenRatioMethod;
}