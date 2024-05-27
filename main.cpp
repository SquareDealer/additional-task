#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <numeric>

using namespace std;

// Структура бактерии

struct Bacteria {
    bool isDead;
    int age;
    int TTL;

    Bacteria(int _TTL) {
        isDead = false;
        TTL = _TTL;
        age = 0;
    }

    void tickDay(int T, int Pminus) {
        age++;
        if(age == TTL) {
            isDead = true;
            return;
        }
        if(age > T / 2) {
            int result = rand() % 100;
            if(result < Pminus) {
                isDead = true;
            }
        }
    }

};

// Функция вывода поля с подсчетом живых и умерших бактерий

void printField(const vector<vector<Bacteria *>> &field, int N) {
    int countAlive = 0;
    int countDead = 0;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if(field[i][j] == nullptr) {
                cout << "* ";
            } else {
                if(field[i][j]->isDead) {
                    cout << "D ";
                    countDead++;
                } else {
                    cout << field[i][j]->age << " ";
                    countAlive++;
                }
            }
        }
        cout << endl;
    }
    cout << "Bacteria count: " << countAlive + countDead << endl;
    cout << "Alive count: " << countAlive << endl;
    cout << "Dead count: " << countDead << endl;
}

// Функция создания поля

void createField(vector<vector<Bacteria *>> &field, int N) {
    for (int i = 0; i < N; i++) {
        vector<Bacteria *> row;
        for (int j = 0; j < N; j++) {
            row.push_back(nullptr);
        }
        field.push_back(row);
    }
}

// Сброс поля - освобождения памяти

void resetField(vector<vector<Bacteria *>> &field, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if(field[i][j] != nullptr) {
                delete field[i][j];
                field[i][j] = nullptr;
            }
        }
    }
}

struct Position {
    int x;
    int y;
};

// Функция создания бактерии

vector<Position> makeBacteria(vector<vector<Bacteria *>> &field, int N, int K1, int K2, int T) {

    Position position;
    vector<Position> positions;

    int countOfBacteria = K1 + rand() % (K2 - K1 + 1);

    for (int i = 0; i < countOfBacteria; i++) {

        int X = rand() % N;
        int Y = rand() % N;

        position.x = X;
        position.y = Y;

        if(field[X][Y] == nullptr) {
            field[X][Y] = new Bacteria(T);
            positions.push_back(position);
        }
    }
    return positions;
}

// Функция создания бактерии в указанной ячейке

void makeBacteriaAtPos(vector<vector<Bacteria *>> &field, int x, int y, int T) {
    field[x][y] = new Bacteria(T);
}

// Функция размножения бактерии

void propagateBacteria(vector<vector<Bacteria*>>& field, int N, int Pplus, int T, int i, int j) {

    // Проходим по 8 соседним ячейкам
    for (int pos = 0; pos < 8; pos++) {
        int x = i, y = j;
        switch (pos) {
            case 0:
                x = i - 1;
                y = j - 1;
            break; // Верхний левый
            case 1:
                x = i - 1;
                y = j;
            break; // Верхний
            case 2:
                x = i - 1;
                y = j + 1;
            break; // Верхний правый
            case 3:
                x = i;
                y = j - 1;
            break; // Левый
            case 4:
                x = i;
                y = j + 1;
            break; // Правый
            case 5:
                x = i + 1;
                y = j - 1;
            break; // Нижний левый
            case 6:
                x = i + 1;
                y = j;
            break; // Нижний
            case 7:
                x = i + 1;
                y = j + 1;
            break; // Нижний правый
        }
        // Проверка границ матрицы
        if (x >= 0 && x < N && y >= 0 && y < N) {
            // Создаем новую бактерию с вероятностью Pplus
            if (field[x][y] == nullptr && (rand() % 100) < Pplus) {
                makeBacteriaAtPos(field, x, y, T);
            }
        }
    }
}

// Field is full check
bool isFieldFull(vector<vector<Bacteria*>>& field, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if(field[i][j] == nullptr) {
                return false;
            }
        }
    }
    return true;
}

// Функция создания среды для бактерий

int petriDish(vector<vector<Bacteria*>>& field, int N, int T, int K1, int K2, int Pplus, int Pminus, int D, int totalDays, vector<Position> &positions) {

    // Цикл дня
    for (int day = 0; day < totalDays; day++) {

        // Tick day
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if(field[i][j] != nullptr)
                    field[i][j]->tickDay(T, Pminus);
            }
        }

        // Заселяем бактерии
        if(day < D) {
            positions = makeBacteria(field, N, K1, K2, T);
            // cout << "Day: " << i << endl;
            // printField(field, N);
            // cout << endl;
        }

        // Propogate stage
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (field[i][j] != nullptr && !field[i][j]->isDead) {
                    if(field[i][j]->age == 1) {
                        propagateBacteria(field, N, Pplus, T, i, j);
                    }
                }
            }
        }
        if(isFieldFull(field, N)) {
            return day;
        }
    }
    // printField(field, N);
    return totalDays;
}

// Функция для симуляции задания 1
void makeSimulationFor1(int N, int K1, int K2, int Pplus, int Pminus, vector<vector<Bacteria *>> field) {
    int D = 7;
    int T = 14;
    vector<int> result; // Результат - кол-во дней
    vector<Position> positions;
    double probabilityOfNotFilled = 0;

    for(int i = 0; i < 1000; i++) {
        int days = petriDish(field, N, T, K1, K2, Pplus, Pminus, D, 5000, positions);
        if(days == 5000) {
            probabilityOfNotFilled++;
        } else {
            result.push_back(days);
        }
        resetField(field, N);

    }
    cout << "T = " << T << ", K1 = " << K1 << ", K2 = " << K2 << ", Pplus = " << Pplus << ", Pminus = " << Pminus << ", D = " << D << endl;

    int minResult = *min_element(result.begin(), result.end());
    cout << "Min days: " << minResult << endl;
    int avgResult = accumulate(result.begin(), result.end(), 0) / result.size();
    cout << "Avg days: " <<avgResult << endl;
    int maxResult = *max_element(result.begin(), result.end());
    cout << "Max days: " << maxResult << endl;
    cout << "Probability of not filled: " << probabilityOfNotFilled/10 << "%" << endl;
    cout << endl;
}

// Функция для симуляции задания 2
void makeSimulationFor2(int N, int T, int K1, int K2, int Pplus, int Pminus, int D, vector<vector<Bacteria *>> field) {
    vector<int> result; // Результат - кол-во дней
    vector<Position> positions; // Набор позиций для текущего эксперимента
    vector<vector<Position>> positionsForExperiment; // Двумерный массив позиций всех эксперементов
    for(int i = 0; i < 100; i++) {
        result.push_back(petriDish(field, N, T, K1, K2, Pplus, Pminus, D, 5000, positions));
        positionsForExperiment.push_back(positions);
        resetField(field, N);
    }

    int min = 10000;
    int minIndex;
    for(int i = 0; i < 100; i++) {
        if(result[i] < min) {
            min = result[i];
            minIndex = i;
        }
    }
    cout << "T = " << T << ", K1 = " << K1 << ", K2 = " << K2 << ", Pplus = " << Pplus << ", Pminus = " << Pminus << ", D = " << D << endl;
    for(int i = 0; i < positionsForExperiment[minIndex].size(); i++) {
        cout << "{X: " << positionsForExperiment[minIndex][i].x << ", Y: " << positionsForExperiment[minIndex][i].y << "}" << endl;
    }
}


int main() {

    srand(time(0));

    vector<vector<Bacteria *>> field;

    Position pos;

    int N = 100;
    int T = 14;
    int K1 = 10000;
    int K2 = 25000;
    int Pplus = 5;
    int Pminus = 2;
    int D = 7;



    createField(field, N);

    // Задание №1

    T = 14;
    D = 7;
    Pplus = 5;
    Pminus = 2;

    makeSimulationFor1(N, K1, K2, Pplus, Pminus, field);
    resetField(field, N);

    N = 100;
    K1 = 10000;
    K2 = 25000;

    makeSimulationFor1(N, K1, K2, Pplus, Pminus, field);
    resetField(field, N);

    N = 50;
    K1 = 2500;
    K2 = 5000;

    makeSimulationFor1(N, K1, K2, Pplus, Pminus, field);
    resetField(field, N);

    N = 25;
    K1 = 500;
    K2 = 600;

    makeSimulationFor1(N, K1, K2, Pplus, Pminus, field);
    resetField(field, N);

    // Задание №2

    N = 30;
    D = 1;
    K1 = 3000;
    K2 = 3000;
    Pplus = 100;
    Pminus = 0;
    T = 5000;

    resetField(field, N);
    makeSimulationFor2(N, T, K1, K2, Pplus, Pminus, D, field);

    return 0;
}
