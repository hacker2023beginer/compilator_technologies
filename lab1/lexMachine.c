#include <stdio.h>
#include <stdlib.h>
#include "header.h"

enum TCharType1{
    Unknown_,
    StartPascal_,
    Zero_,
    ContinueC_,
    Digit_,
    Letters_,
    TCharTypeCount_ //количество состояний
};

const int isFinalState[] = {0, 0, 0, 1};
const int values[4][TCharTypeCount] = {{0, 0, 0, 0}, {0, 2, 0, 0}, {0, 0, 3, 3}, {0, 0, 3, 3}};

int getCharType(char *symbol, int *addC); 
int getInput(char **str);
int checkString(char str[], size_t length);
void * checkRealloc(void *pointer, size_t newLength);
void findCandPascalNums(char *str);
void grow(char **str, int *capacity);
void printSubstrings(char *pascalStrings, int length_Pascal, char *cStrings, int length_C);

int main(int argc, char const *argv[])
{
    while (1){
        size_t len = 0;
        char *string;
        printf("Input your string: ");
        len = getInput(&string);
        if (isFinalState[checkString(string, len)]){
            if (*string == '$') printf("It is a Pascal hex num \n");
            else printf("It is a C hex num \n");
        }
        else {
            printf("It is not a hex num in C or Pascal \n");
        }
        findCandPascalNums(string);
        free(string);
    }
    return 0;

}

//функция для ввода строки, возвращает длину
int getInput(char **string)
{
    char symbol;
    char *str;
    size_t capacity = 16, length = 0;
    str = calloc(capacity, sizeof(char));
    while ((symbol = getchar()) != '\n')      
    { 
        if (length == capacity) //if (length > capacity - 2)
        {
            capacity = capacity << 1;
            str = checkRealloc(str, capacity);
        }
        str[length] = symbol;
        length++; //только для Char 1 байт
    };
    str = checkRealloc(str, length + 1);
    *string = str;
    return length;
}

void * checkRealloc(void *pointer, size_t newLength){
    void *temp;
    temp = realloc(pointer, newLength);
    if (temp != NULL) return temp;
    else{
        printf("Stopped because of OutOfMemory");
        exit(1);
    }
}

int getCharType(char *symbol, int *addC){
    /*
    Возвращаем номер класса, которому принадлежит текущий элемент:
    Letters ::=  [A..Fa..f];
    Digit ::= [0..9];
    StartN ::= ($, 0x, 0X).
    */
   *addC = 0;
   char elem = *symbol;
   if (elem == '$') return StartN;
   if (((elem >= 'A') && (elem <= 'F')) || ((elem >= 'a') && (elem <= 'f'))) return Letters;
   else if (elem == '0') {
        if (*(symbol + 1) != '\0'){ //проверяем, можем ли мы проверить следующий символ
            char nextElem = *(symbol + 1);
            if ((nextElem == 'x') || (nextElem == 'X')){
                *addC = 1;
                return StartN;
            }
        }
        return Digit;
   }
   else if ((elem >= '1') && (elem <= '9')) return Digit;
   else return Unknown;
}

int checkString(char str[], size_t length){
    int state = 1,  *addC;
    addC = malloc(sizeof(int));
    *addC = 0;
    for (size_t i = 0; i < length; i++){
        state = values[state][getCharType(str + i + *addC, addC)];
    }
    free(addC);
    return state;
}

void grow(char **str, int *capacity){
    *capacity = (*capacity) * 2;
    *str = (char *) checkRealloc(*str, *capacity);
}

void findCandPascalNums(char *str){
    int i = 0, *addC, state = 1, typeOfChar, indexStartNum, isCstring;
    addC = malloc(sizeof(int));
    *addC = 0;

    int *C_capacity, *Pascal_capacity, length_C, length_Pascal;
    length_C = 0;
    length_Pascal = 0;
    C_capacity = malloc(sizeof(int));
    Pascal_capacity = malloc(sizeof(int));
    *C_capacity = 16;
    *Pascal_capacity = 16;

    char *cStrings, *pascalStrings; //надо память выделять под них и потом записывать туда соотв строки
    cStrings = malloc(*C_capacity);
    pascalStrings = malloc(*Pascal_capacity);
    while (str[i] != '\0'){
        typeOfChar = getCharType(str + i/* + *addC*/, addC);
        state = values[state][typeOfChar];
        if (isFinalState[state]){
            if (isCstring){
                for (int index = indexStartNum; index <= i; index++){
                    if (length_C == *C_capacity) grow(&cStrings, C_capacity);
                    cStrings[length_C] = str[index]; 
                    length_C++;
                }    
            }
            else {
                for (int index = indexStartNum; index <= i; index++){
                    if (length_Pascal == *Pascal_capacity) grow(&pascalStrings, Pascal_capacity);
                    pascalStrings[length_Pascal] = str[index]; 
                    length_Pascal++;
                } 
            }
        }
        else if ((state == 0) && (typeOfChar != StartN)) state = 1;
        else if ((typeOfChar == StartN) && (state == 2)){
            indexStartNum = i;
            isCstring = *addC;
        }
        else if ((state == 0) && (typeOfChar == StartN)){
            //isCstring = *addC;
            if (*addC){
                if (isCstring) {
                    for (int index = indexStartNum; index <= i; index++){
                        if (length_C == *C_capacity) grow(&cStrings, C_capacity);
                        cStrings[length_C] = str[index]; 
                        length_C++;
                    } 
                }
                else {
                    for (int index = indexStartNum; index <= i; index++){
                        if (length_Pascal == *Pascal_capacity) grow(&pascalStrings, Pascal_capacity);
                        pascalStrings[length_Pascal] = str[index]; 
                        length_Pascal++;
                    }     
                }
            }
            state = 1;
            continue;
        }
        i = i + 1 + *addC;
    }
    printSubstrings(pascalStrings, length_Pascal, cStrings, length_C);

    //избавляемся от утечек памяти
    free(addC);
    free(C_capacity);
    free(Pascal_capacity);
    free(cStrings);
    free(pascalStrings);
}

void printSubstrings(char *pascalStrings, int length_Pascal, char *cStrings, int length_C){
    //Вывод паскаль чисел
    if (length_Pascal != 0){
        printf("Founded Pascal substrings:\n");
        for (int i = 0; i < length_Pascal; i++){
            if ((pascalStrings[i] == '$') && (i != 0)) printf("\n%c", pascalStrings[i]);
            else printf("%c", pascalStrings[i]);
        }
        printf("\n\n");
    }
    else printf("There arent any Pascal substrings\n\n");

    //Вывод си чисел
    if (length_C != 0){
        printf("Founded C substrings:\n");
        int *addC;
        addC = malloc(sizeof(int));
        *addC = 0;
        for (int i = 0; i < length_C; i++){
            if ((getCharType(cStrings + i, addC) == StartN) && (i != 0)) printf("\n%c", cStrings[i]);
            else printf("%c", cStrings[i]);
        }
        printf("\n\n");
        free(addC);
    }
    else printf("There arent any C substrings\n\n");
}


























// int read_line_from_file(const char* filename, char* buf, size_t buf_size) {
//     FILE* f = fopen(filename, "r");
//     if (!f) {
//         perror("Не удалось открыть файл");
//         return 0;
//     }

//     if (!fgets(buf, buf_size, f)) {
//         fclose(f);
//         return 0; // ошибка или пустой файл
//     }

//     // Убираем перевод строки, если есть
//     size_t len = strlen(buf);
//     if (len > 0 && buf[len - 1] == '\n') {
//         buf[len - 1] = '\0';
//     }

//     fclose(f);
//     return 1;
// }
// if (!read_line_from_file("D:\\ktlabs\\lab1\\output\\input.txt", s, sizeof(s))) {
//     printf("Ошибка чтения строки из файла\n");
//     return 1;
// }