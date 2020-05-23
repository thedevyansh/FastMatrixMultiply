#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define MAX 100

int sizeA = 0, sizeB = 0;

int input(int arr[][3], int, int);
FILE* Combined_Sparse_Compact(int A[][3], int B[][3]);

int main()
{
    int r1, r2, c1, c2;
    int A[MAX][3], B[MAX][3];

    printf("Enter number of rows and columns of big sparse matrix 1: ");
    scanf("%d %d", &r1, &c1);
    printf("Enter the values of big sparse matrix 1:\n");
    sizeA = input(A, r1, c1);

    printf("Enter number of rows and columns of big sparse matrix 2: ");
    scanf("%d %d", &r2, &c2);
    printf("Enter the values of big sparse matrix 2:\n");
    sizeB = input(B, r2, c2);

    FILE *fp = Combined_Sparse_Compact(A, B);

    return 0;
}

int input(int arr[][3], int r, int c)
{
    int num, k = 0;

    for(int i = 0; i < r; i++) {

        for(int j = 0; j < c; j++) {

            scanf("%d", &num);

            if(num != 0) {
                arr[k][0] = i;
                arr[k][1] = j;
                arr[k][2] = num;
                k++;
            }
        }
    }

   return k;
}

FILE* Combined_Sparse_Compact(int A[][3], int B[][3])
{
    int i, j;
    char str1[MAX], str2[MAX], temp[10];
    char line[2*MAX];

    FILE *f1 = fopen("fileA.txt", "a");

    if(!f1)
      exit(1);

    FILE *f2 = fopen("fileB.txt", "a");
    if(!f2)
      exit(1);


    for(i = 0; i < sizeA; i++) {

        if(A[i][2] != 0) {

           str1[0] = '\0'; strcat(str1, "A,");
           line[0] = '\0';
           str2[0] = '\0';
           sprintf(temp, "%d", A[i][0]);
           strcat(str1, temp);
           strcat(str1, ",");

           for(j = i; A[j][0] == A[i][0]; j++) {

            sprintf(temp, "%d", A[j][1]);
            strcat(str1, temp);
            strcat(str1, ",");
            sprintf(temp, "%d", A[j][2]);
            strcat(str2, temp);
            strcat(str2, ",");

            A[j][2] = 0;
           }

           str1[strlen(str1)-1] = '\0';
           str2[strlen(str2)-1] = '\0';

           strcat(line, str1);
           strcat(line, "   ");
           strcat(line, str2);

           fprintf(f1, "%s\n", line);

        }
    }

    fclose(f1);

    for(i = 0; i < sizeB; i++) {

        if(B[i][2] != 0) {

           str1[0] = '\0'; strcat(str1, "B,");
           line[0] = '\0';
           str2[0] = '\0';
           sprintf(temp, "%d", B[i][0]);
           strcat(str1, temp);
           strcat(str1, ",");

           for(j = i; B[j][0] == B[i][0]; j++) {

            sprintf(temp, "%d", B[j][1]);
            strcat(str1, temp);
            strcat(str1, ",");
            sprintf(temp, "%d", B[j][2]);
            strcat(str2, temp);
            strcat(str2, ",");

            B[j][2] = 0;
           }

           str1[strlen(str1)-1] = '\0';
           str2[strlen(str2)-1] = '\0';

           strcat(line, str1);
           strcat(line, " ");
           strcat(line, str2);

           fprintf(f2, "%s\n", line);

        }
    }

    fclose(f2);

    f1 = fopen("fileA.txt", "r");

    if(!f1)
      exit(1);

    f2 = fopen("fileB.txt", "r");
    if(!f2)
      exit(1);

    FILE *D = fopen("fileCombined.txt", "w");
    if(!D)
        exit(1);

    char ch;

    while((ch = getc(f1)) != EOF)
        fputc(ch, D);

    while((ch = getc(f2)) != EOF)
        fputc(ch, D);

    fclose(f1); fclose(f2);
    remove("fileA.txt"); remove("fileB.txt");

    return D;

}
