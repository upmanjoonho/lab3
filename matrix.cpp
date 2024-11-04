#include <iostream>
#include <vector>

using namespace std;

int main() {
    int rows, cols;

    cout << "첫 번째 행렬의 행과 열의 크기를 입력하세요 (예: 2 3): ";
    cin >> rows >> cols;

    vector<vector<int>> matrixA(rows, vector<int>(cols));
    vector<vector<int>> matrixB(rows, vector<int>(cols));
    vector<vector<int>> sumMatrix(rows, vector<int>(cols));

    cout << "첫 번째 행렬의 원소를 입력하세요:\n";
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            cout << "A[" << i << "][" << j << "] = ";
            cin >> matrixA[i][j];
        }
    }

    cout << "두 번째 행렬의 원소를 입력하세요:\n";
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            cout << "B[" << i << "][" << j << "] = ";
            cin >> matrixB[i][j];
        }
    }

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            sumMatrix[i][j] = matrixA[i][j] + matrixB[i][j];
        }
    }

    cout << "두 행렬의 합:\n";
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            cout << sumMatrix[i][j] << " ";
        }
        cout << endl;
    }

    return 0;
}
