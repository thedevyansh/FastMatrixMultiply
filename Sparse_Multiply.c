/*
COPYRIGHT 2020 DEVYANSH CHAWLA. All Rights Reserved.
Created from May 21, 2020 to May 26, 2020.
*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define MAX 500

//Number of non zero values in big data sparse matrices A & B
int sizeA = 0, sizeB = 0;

//Number of rows and columns in a & B
int r1, r2, c1, c2;

//user defined data type for storing key-value pairs
struct list
{
	char keyValue[50];
	char elementalValues[5][10];
	struct list *next;
};


int input(int arr[][3], int, int);
FILE* Combined_Sparse_Compact(int A[][3], int B[][3]);
FILE* FAST_MAP_sparseMUL(FILE* D);
FILE* FAST_RED_sparseMUL (FILE* sorted);

int subString(char el[][10], char *str, char *ch);
void pointerTostring(char *str, char *ptr);
FILE* sortedDpart(FILE* Dpart);
int keyCompare(char str1[][10], char str2[][10]);


int main()
{
    int A[MAX][3], B[MAX][3];

    printf("Enter number of rows and columns of big sparse matrix 1: ");
    scanf("%d %d", &r1, &c1);
    printf("Enter the values of big sparse matrix 1:\n");
    sizeA = input(A, r1, c1);

    printf("Enter number of rows and columns of big sparse matrix 2: ");
    scanf("%d %d", &r2, &c2);
    printf("Enter the values of big sparse matrix 2:\n");
    sizeB = input(B, r2, c2);

    //function called to get compact mapreducible format of matrices A & B
    FILE *fp = Combined_Sparse_Compact(A, B);

    //called mapping function
    FILE *Dpart = FAST_MAP_sparseMUL(fp);

    //called sorting function to sort intermediate key-value pairs file
    FILE *sorted = sortedDpart(Dpart);

    //called reduce function which will return the final result
    FILE *finalResult = FAST_RED_sparseMUL(sorted);

    fclose(finalResult);

    printf("\nHurray! The multiplication is done. Check the result in finalResult.txt file in the same directory!\n");

    return 0;
}

//function to input values of sparse matrices A & B without storing null values
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

//function to represent A & B in compact mapreducible format
FILE* Combined_Sparse_Compact(int A[][3], int B[][3])
{
    int i, j;
    char str1[MAX], str2[MAX], temp[50];
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

//function to get intermediate key-value pairs
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
        char *str[4], *c;
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
        char temp[50];

        char key[50], value[50];

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

                    fprintf(Dpart, "%s,%s\n", key, value);
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

                    fprintf(Dpart, "%s,%s\n", key, value);
                }
            }
        }

    }

    fclose(D);
    free(strBase);

    return Dpart;

}

//function to copy contents of a string in another character array
void pointerTostring(char *str , char *ptr)
{

	int i = 0 ;

	while(*ptr!='\0') {

		str[i] = *ptr;
		i++;  ptr++;
	}

	str[i] = '\0';
}

//function to split a given string according to a delimiter
int subString(char el[][10], char *str, char *ch)
{
	int i = 0;
	char *ptr;

	ptr = strtok(str,ch);

	while(ptr != NULL) {

	  	pointerTostring (el[i], ptr);
	  	ptr = strtok(NULL,ch);
	  	i++;

	}

	return i ;
}

//function to compare key values
int keyCompare(char str1[5][10], char str2[5][10])
{

	if(atoi(str1[0]) > atoi(str2[0]))
		return 1  ;

	else if(atoi(str1[0]) < atoi(str2[0]))
		return -1 ;

	else  {

		if(atoi(str1[1]) > atoi(str2[1]))
			return 1 ;

		else if (atoi(str1[1]) < atoi(str2[1]))
			return -1 ;

		else
			return 0 ;

	}
}

//function to sort the intermediate key-value pairs according to the keys
FILE* sortedDpart(FILE *Dpart)
{

    fclose(Dpart);

	Dpart = fopen("keyValue.txt","r");

	FILE *sorted = fopen("sorted.txt","w");

	char str[50], str1[50], str2[50];
	struct list *head = NULL, *temp, *iTemp, *key, *node;
	int n = 0, nCount, isGreater, isSwapped, len, i ;

	while(fgets(str, 50, Dpart) != NULL)  {

        n++;
	    len = strlen(str);
		str[len-1] = '\0';

		node = (struct list *) malloc(sizeof(struct list));
		node->next = NULL;
		strcpy(node->keyValue,str);

		nCount = subString(node->elementalValues, str, ",");

		if(head == NULL) {
			head = node;
			temp = head;
		}

		else {
			temp->next = node;
			temp = temp->next;
		}
    }

    temp = NULL;

    while(temp != head->next) {

    	isSwapped = 0;
    	key = head;
    	iTemp = key->next;

    	while(iTemp != temp) {

    		isGreater = keyCompare(key->elementalValues, iTemp->elementalValues);

    		if(isGreater == 1) {

    			isSwapped = 1;

    			strcpy(str, key->keyValue);
    			strcpy(key->keyValue, iTemp->keyValue);
    			strcpy(iTemp->keyValue, str);

    			nCount = subString(iTemp->elementalValues, str, ",");

    			strcpy(str,key->keyValue) ;

    			nCount = subString(key->elementalValues, str, ",");
    		}

    		key = key->next ;
    		iTemp = iTemp->next ;
    	}

   		temp = key;

    	if(isSwapped == 0)
    		break;
	}

	temp = head;

	while(temp != NULL) {

		fputs(temp->keyValue,sorted) ;
		temp = temp->next ;
		fprintf(sorted, "%c", '\n');
	}

	fclose(Dpart) ;
	fclose(sorted) ;

	return sorted;
}

//final reduce function to get the result of multiplication of A & B
FILE* FAST_RED_sparseMUL(FILE *sorted)
{

    fclose(sorted);

    sorted = fopen("sorted.txt","r");

	FILE *result = fopen("finalResult.txt","w");

	struct list *head1 = NULL, *head2 = NULL, *temp1, *temp2, *node ;
	int i, j, len, count1 = 0, count2 = 0, nCount, val1, val2, val=0, flag;
	char str[50], str1[10], str2[10], tempStr[10];

	while(fgets(str,50,sorted) != NULL) {

		len=strlen(str);
		str[len-1]='\0';

		node=(struct list *)malloc(sizeof(struct list));
		node->next=NULL;

		strcpy(node->keyValue,str);

		nCount = subString(node->elementalValues, str,",");

		if(strcmp(node->elementalValues[2], "A") == 0) {

			if(head1 == NULL) {
				head1 = node ;
				temp1 = head1 ;
			}

			temp1->next = node ;
			temp1 = temp1->next ;
			count1++ ;
		}

		else if(strcmp(node->elementalValues[2], "B") == 0 ) {

			if(head2 == NULL) {
				head2 = node;
				temp2 = head2;
			}

			temp2->next = node;
			temp2 = temp2->next;
			count2++;
		}
	}

	temp1 = head1;
	temp2 = head2;

	while(temp1 != NULL && temp2 != NULL) {

		val = 0;

		strcpy(str1,temp1->elementalValues[0]);
		strcpy(str2,temp1->elementalValues[1]);

		while(temp1 != NULL && temp2 != NULL && keyCompare(temp1->elementalValues, temp2->elementalValues) == 0
        && strcmp(temp1->elementalValues[0], str1) == 0 && strcmp(temp1->elementalValues[1], str2)==0) {

			if(strcmp(temp1->elementalValues[3], temp2->elementalValues[3]) == 0) {

				val1 = atoi(temp1->elementalValues[4]);
			    val2 = atoi(temp2->elementalValues[4]);
			    val += val1*val2 ;
			    temp1 = temp1->next;
			    temp2 = temp2->next;

			}

			else if(atoi(temp1->elementalValues[3]) < atoi(temp2->elementalValues[3])) {

				temp1 = temp1->next;

			}

			else {

				temp2 = temp2->next;
			}
		}

		if(val != 0) {

			strcpy(str, str1);
			strcat(str, ",");
			strcat(str, str2);
			strcat(str, ",");
			itoa(val, tempStr,10);
			strcat(str, tempStr);
			fputs(str, result);
			fprintf(result, "%c", '\n');
		}

		while(temp1 != NULL && temp2 != NULL && keyCompare(temp1->elementalValues, temp2->elementalValues) < 0 ) {
			temp1 = temp1->next;
		}

		while(temp1 != NULL && temp2!=NULL && keyCompare(temp1->elementalValues, temp2->elementalValues) > 0 ) {
			temp2 = temp2->next;
		}
	}

	fclose(result) ;

	return result;
}
