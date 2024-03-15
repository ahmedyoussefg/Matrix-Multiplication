#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define MAX_SIZE 21

int a[MAX_SIZE][MAX_SIZE];
int b[MAX_SIZE][MAX_SIZE];
int c_per_matrix[MAX_SIZE][MAX_SIZE];
int c_per_row[MAX_SIZE][MAX_SIZE];
int c_per_element[MAX_SIZE][MAX_SIZE];
int x, y, m, n;
void threadPerMatrix();
void threadPerRow();
void threadPerElement();
void *computeRow(void *ptr);
void *computeElement(void *ptr);
void readFromFile(FILE *file, int *rows, int *cols, int matrix[MAX_SIZE][MAX_SIZE]);
typedef struct axes
{
    int i;
    int j;
} axes;

int main()
{
    FILE *input_a=fopen("a.txt","r");
    if(input_a==NULL)
    {
        printf("File first input not found\n");
        exit(1);
    }
    readFromFile(input_a, &x, &y, a);
    fclose(input_a);
    FILE *input_b=fopen("b.txt","r");
    if(input_b==NULL)
    {
        printf("File second input not found\n");
        exit(1);
    }
    readFromFile(input_b, &m, &n, b);
    fclose(input_b);

    printf("Matrix A dims: \n");
    printf("%d %d\n", x, y);
    printf("Matrix B dims: \n");
    printf("%d %d\n", m, n);
    printf("Matrix A\n");

    for (int i = 0; i < x; i++)
    {
        for (int j = 0; j < y; j++)
        {
            printf("%d ", a[i][j]);
        }
        printf("\n");
    }
    printf("Matrix B\n");
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%d ", b[i][j]);
        }
        printf("\n");
    }

    // struct timeval stop, start;
    // gettimeofday(&start, NULL); //start checking time

    // printf("Matrix C (thread per matrix)\n");        
    // threadPerMatrix();

    // gettimeofday(&stop, NULL); //end checking time
    // printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    // printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);


    // gettimeofday(&start, NULL); //start checking time
    // printf("Matrix C (thread per row)\n");     

    // threadPerRow();

    // gettimeofday(&stop, NULL); //end checking time
    // printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    // printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);


    // gettimeofday(&start, NULL); //start checking time
    // printf("Matrix C (thread per element)\n");   

    // threadPerElement();
    
    // gettimeofday(&stop, NULL); //end checking time
    // printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    // printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
}

void threadPerMatrix()
{
    int row = x, col = n;
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            for (int k = 0; k < m; k++)
            {
                c_per_matrix[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

void threadPerRow()
{
    int row = x, col = n;
    pthread_t threads[row];
    for (int i = 0; i < row; i++)
    {
        int *ptr_to_i = (int *)malloc(sizeof(int));
        if (ptr_to_i == NULL)
        {
            printf("Memory allocation failed\n");
            exit(1);
        }
        *ptr_to_i = i;

        pthread_create(&threads[i], NULL, computeRow, (void *)ptr_to_i);
    }
    for (int i = 0; i < row; i++)
    {
        pthread_join(threads[i], NULL);
    }
}
void *computeRow(void *ptr)
{
    int *ptr_row = (int *)ptr;
    int curr_row = *ptr_row;
    for (int j = 0; j < y; j++)
    {
        for (int k = 0; k < m; k++)
        {
            c_per_row[curr_row][j] += a[curr_row][k] * b[k][j];
        }
    }
    free(ptr);
}
void threadPerElement()
{
    int row = x, col = n;
    pthread_t threads[row][col];
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            axes *curr_ptr = (axes *)malloc(sizeof(axes));
            if (curr_ptr == NULL)
            {
                printf("Memory allocation failed\n");
                exit(1);
            }
            curr_ptr->i=i;
            curr_ptr->j=j;
            pthread_create(&threads[i][j], NULL, computeElement, (void *)curr_ptr);
        }
    }
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            pthread_join(threads[i][j], NULL);
        }
    }
}
void *computeElement(void *ptr)
{
    axes *curr_ptr = (axes *)ptr;
    int i = curr_ptr->i;
    int j = curr_ptr->j;
    for (int k = 0; k < m; k++)
    {
        c_per_element[i][j] += a[i][k] * b[k][j];
    }
    free(ptr);
}
void readFromFile(FILE *file, int *rows, int *cols, int matrix[MAX_SIZE][MAX_SIZE]) {
    fscanf(file, "row=%d col=%d\n", rows, cols);
    for (int i = 0; i < *rows; i++) {
        for (int j = 0; j < *cols; j++) {
            if (fscanf(file, "%d", &matrix[i][j]) != 1) {
                perror("Error reading matrix");
                exit(1);
            }
        }
    }
}