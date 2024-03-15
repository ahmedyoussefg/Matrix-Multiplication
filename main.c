#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_SIZE 21

int a[MAX_SIZE][MAX_SIZE];
int b[MAX_SIZE][MAX_SIZE];
int c_per_matrix[MAX_SIZE][MAX_SIZE];

void threadPerMatrix(int x, int y, int m, int n);
int main() {
    int x, y;
    printf("Matrix A dims: \n");
    scanf("%d %d", &x, &y);
    int m, n;
    printf("Matrix B dims: \n");
    scanf("%d %d", &m , &n);
    printf("Matrix A\n");

    for (int i =0;i < x; i++){
        for (int j =0;j< y;j++){
            scanf("%d", &a[i][j]);
        }
    }
    printf("Matrix B\n");
    for (int i =0;i < m; i++){
        for (int j =0;j< n;j++){
            scanf("%d", &b[i][j]);
        }
    }

    threadPerMatrix(x,y,m,n);
    for (int i =0;i<x;i++){
        printf("[ ");
        for (int j =0;j <n;j++){
            printf("%d ", c_per_matrix[i][j]);
        }
        printf("]\n");
    }
}

void threadPerMatrix(int x, int y, int m, int n) {
    int row=x, col = n;
    for (int i = 0; i < row; i++){
        for (int j = 0; j < col; j++){
            for (int k = 0; k < m; k++){
                c_per_matrix[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}