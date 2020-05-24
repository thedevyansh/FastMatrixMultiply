#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define MAX 100

int sizeA = 0, sizeB = 0;
int r1, r2, c1, c2;

//doing all this shit for creating two hashtables A & B
struct node
{
    int key;
    float value;
    struct node *next;
};

struct hashTable
{
    int size;
    struct node **list;
};

struct hashTable *tA = NULL, *tB = NULL;

int input(int arr[][3], int, int);
FILE* Combined_Sparse_Compact(int A[][3], int B[][3]);
FILE* FAST_MAP_sparseMUL(FILE* D); // Map task;
void FAST_RED_sparseMUL (FILE* Dpart); // Reduce task ;

struct hashTable* createTable(int);
int hashCode(struct hashTable*, int);
void insert(struct hashTable*, int, float);

int main()
{
    //int r1, r2, c1, c2;
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

    FILE *Dpart = FAST_MAP_sparseMUL(fp);  //calling map function

    //creating two hashtables for A and B
    tA = createTable(MAX);
    tB = createTable(MAX);

    FAST_RED_sparseMUL(Dpart); // calling reduce function

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
           strcat(line, "\t");
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
           strcat(line, "\t");
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

FILE* FAST_MAP_sparseMUL(FILE* D)
{
    int count = 0, l = 0;
    char ch;

    fclose(D);

    D = fopen("fileCombined.txt", "r");

    FILE* Dpart = fopen("keyValue.txt", "w");

    while((ch = getc(D)) != EOF) {
        if(ch == '\n')
            count++;
    }

    rewind(D);

    char *strBase[count];

    for(int i = 0; i < count; i++) {
        strBase[i] = (char *) malloc(2 * MAX * sizeof(char));
        strBase[i][0] = '\0';
    }

    while((ch = getc(D)) != EOF) {

       if(ch !='\n') {
       strncat(strBase[l], &ch, 1);
       }

       else
       l++;
    }

    for(int i = 0; i < count; i++) {

        l = 0;
        char *str[2], *c;
        int s1, s2;

        c = strtok(strBase[i], "\t");
        while(c != NULL) {
            str[l++] = c;
            c = strtok(NULL, "\t");
        }
        l = 0;

        char *str1[strlen(str[0])], *str2[strlen(str[1])];

        c = strtok(str[0], ",");
        while(c != NULL) {
            str1[l++] = c;
            c = strtok(NULL, ",");
        }
        s1 = l;
        l = 0;

        c = strtok(str[1], ",");
        while(c != NULL) {
            str2[l++] = c;
            c = strtok(NULL, ",");
        }
        s2 = l;
        char temp[10];

        //need to improve limits of key and value
        char key[10], value[10];

        if(strcmp(str1[0], "A") == 0) {

            for(int j = 0; j < c2; j++) {

                for(int r = 0; r < s2; r++) {

                    key[0] = '\0';
                    strcat(key, str1[1]);
                    strcat(key, ",");
                    sprintf(temp, "%d", j);
                    strcat(key, temp);

                    value[0] = '\0';
                    strcat(value, "A");
                    strcat(value, ",");
                    strcat(value, str1[r+2]);
                    strcat(value, ",");
                    strcat(value, str2[r]);

                    fprintf(Dpart, "%s\t%s\n", key, value);
                }
            }
        }

        else if(strcmp(str1[0], "B") == 0) {

            for(int j = 0; j < r1; j++) {

                for(int r = 0; r < s2; r++) {

                    key[0] = '\0';
                    sprintf(temp, "%d", j);
                    strcat(key, temp);
                    strcat(key, ",");
                    strcat(key, str1[r+2]);

                    value[0] = '\0';
                    strcat(value, "B");
                    strcat(value, ",");
                    strcat(value, str1[1]);
                    strcat(value, ",");
                    strcat(value, str2[r]);

                    fprintf(Dpart, "%s\t%s\n", key, value);
                }
            }
        }

    }

    fclose(D); remove("fileCombined.txt");
    free(strBase);

    return Dpart;

}

// H A S H      T A B L E       C R E A T I O N
struct hashTable* createTable(int size)
{
    struct hashTable *t = (struct hashTable*) malloc(sizeof(struct hashTable));
    t->size = size;
    t->list = (struct node**) malloc(size * sizeof(struct node *));

    for(int i = 0; i < size; i++)
        t->list[i] = NULL;

    return t;
}

// H A S H      C O D E
int hashCode(struct hashTable* t, int key)
{
    return key%(t->size);
}

//INSERTING IN HASH TABLE
void insert(struct hashTable* t, int key, float value)
{
    int pos = hashCode(t, key);

    struct node *temp;

    if(t->list[pos] == NULL) {
        struct node *newnode = (struct node*) malloc(sizeof(struct node));
        newnode->key = key;
        newnode->value = value;
        newnode->next = NULL;

        t->list[pos] = newnode;
        temp = newnode;
    }

    else {

        struct node *transverse = t->list[pos];

        while(transverse != NULL) {
            if(transverse->key == key) {
                transverse->value = value;
                return;
            }
            transverse = transverse->next;
        }

        struct node *newnode = (struct node*) malloc(sizeof(struct node));
        newnode->key = key;
        newnode->value = value;
        newnode->next = NULL;

        temp->next = newnode;
        temp = temp->next;
    }
}

void FAST_RED_sparseMUL(FILE* Dpart)
{
    fclose(Dpart);

    Dpart = fopen("keyValue.txt", "r");

    int count = 0, i, l = 0;
    char ch;

    while((ch = getc(Dpart)) != EOF) {
        if(ch == '\n')
            count++;
    }
    rewind(Dpart);

    char *strTemp[count];

    for(i = 0; i < count; i++) {
        strTemp[i] = (char *) malloc(sizeof(char) * 2 * MAX);
        strTemp[i][0] = '\0';
    }

    while((ch = getc(Dpart)) != EOF) {
        if(ch != '\n')
        strncat(strTemp[l], &ch, 1);

        else
        l++;
    }

    for(i = 0; i < count; i++) {

        char *str[2], *c;
        l = 0;

        c = strtok(strTemp[i], "\t");
        while(c != NULL) {
            str[l++] = c;
            c = strtok(NULL, "\t");
        }
        l = 0;

        char *str1[3];

        c = strtok(str[1], ",");
        while(c != NULL) {
            str1[l++] = c;
            c = strtok(NULL, ",");
        }

        if(strcmp(str1[0], "A") == 0) {

            //insert in hashtable A
            insert(tA, atoi(str1[1]), atof(str1[2]));
        }

        else if(strcmp(str1[0], "B") == 0) {

            //insert in hashtable B
            insert(tB, atoi(str1[1]), atof(atr1[2]));
        }

    }
}
