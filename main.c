#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>

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
void writeMatrixToFile(FILE *FILE, int matrix[MAX_SIZE][MAX_SIZE]);
typedef struct axes
{
    int i;
    int j;
} axes;

int main(int argc, char *argv[])
{
    char *first_input="a.txt";
    char *second_input="b.txt";
    char *outputs[]={"c","c","c"};
    char *suffixes[]={"_per_matrix.txt","_per_row.txt","_per_element.txt"};
    if(argc>1){
        first_input=argv[1];
        char *new_input = malloc(strlen(first_input) + 5); 
        if (new_input == NULL) {
            printf("Memory allocation failed\n");
            exit(1);
        }
        strcpy(new_input, first_input);
        strcat(new_input, ".txt");
        first_input=new_input;
    }
    if (argc>2){
        second_input=argv[2];
        char *new_input = malloc(strlen(second_input) + 5);
        if (new_input == NULL) {
            printf("Memory allocation failed\n");
            exit(1);
        }
        strcpy(new_input, second_input);
        strcat(new_input, ".txt");
        second_input=new_input;;
    }
    if (argc>3){
        for (int i =0;i < 3;i++)
            outputs[i]=argv[3];  
    }
    for (int i=0;i<3;i++){
        char *new_output = malloc(strlen(outputs[i]) + strlen(suffixes[i]) + 1);
        if (new_output == NULL) {
            printf("Memory allocation failed\n");
            exit(1);
        }
        strcpy(new_output, outputs[i]);
        strcat(new_output, suffixes[i]);
        outputs[i]=new_output;
    }
    FILE *input_a=fopen(first_input,"r");
    if(input_a==NULL)
    {
        printf("File first input not found\n");
        exit(1);
    }
    readFromFile(input_a, &x, &y, a);
    fclose(input_a);
    FILE *input_b=fopen(second_input,"r");
    if(input_b==NULL)
    {
        printf("File second input not found\n");
        exit(1);
    }
    readFromFile(input_b, &m, &n, b);
    fclose(input_b);
    if (y!=m){
        printf("Matrix A and B are not compatible for multiplication\n");
        exit(1);
    }

    struct timeval stop, start;
    gettimeofday(&start, NULL); //start checking time

    printf("Matrix C (thread per matrix)\n");        
    threadPerMatrix();

    gettimeofday(&stop, NULL); //end checking time
    printf("Number Of Threads created: 1\n");
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    printf("----------------------------------\n");
    FILE *output_by_matrix=fopen(outputs[0],"w");
    
    if(output_by_matrix==NULL)
    {
        printf("Error writing to file\n");
        exit(1);
    }
    fprintf(output_by_matrix, "Method: A thread per matrix\n");
    fprintf(output_by_matrix, "row=%d col=%d\n", x, n);
    writeMatrixToFile(output_by_matrix,c_per_matrix);
    fclose(output_by_matrix);

    gettimeofday(&start, NULL); //start checking time
    printf("Matrix C (thread per row)\n");     

    threadPerRow();

    gettimeofday(&stop, NULL); //end checking time
    printf("Number Of Threads created: %d\n", x);

    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    printf("----------------------------------\n");

    FILE *output_by_row=fopen(outputs[1],"w");
    
    if(output_by_row==NULL)
    {
        printf("Error writing to file\n");
        exit(1);
    }
    fprintf(output_by_row, "Method: A thread per row\n");
    fprintf(output_by_row, "row=%d col=%d\n", x, n);
    writeMatrixToFile(output_by_row,c_per_row);
    fclose(output_by_row);

    gettimeofday(&start, NULL); //start checking time
    printf("Matrix C (thread per element)\n");   

    threadPerElement();
    
    gettimeofday(&stop, NULL); //end checking time
    printf("Number Of Threads created: %d\n", x*n);

    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    FILE *output_by_element=fopen(outputs[2],"w");
    
    if(output_by_element==NULL)
    {
        printf("Error writing to file\n");
        exit(1);
    }
    fprintf(output_by_element, "Method: A thread per element\n");
    fprintf(output_by_element, "row=%d col=%d\n", x, n);
    writeMatrixToFile(output_by_element,c_per_element);
    fclose(output_by_element);
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
    for (int j = 0; j < n; j++)
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
void writeMatrixToFile(FILE *FILE, int matrix[MAX_SIZE][MAX_SIZE]){
    for (int i = 0; i < x; i++)
    {
        for (int j = 0; j < n; j++)
        {
            fprintf(FILE, "%d ", matrix[i][j]);
        }
        if (i!=x-1)
            fprintf(FILE, "\n");
    }
}