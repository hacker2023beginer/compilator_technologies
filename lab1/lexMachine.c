#include <stdio.h>
#include <stdlib.h>

enum TCharType{
    Unknown,
    StartN,
    Digit,
    Letters,
    TCharTypeCount //количество состояний
};

const int isFinalState[] = {0, 0, 0, 1};
const int values[4][TCharTypeCount] = {{0, 0, 0, 0}, {0, 2, 0, 0}, {0, 0, 3, 3}, {0, 0, 3, 3}};

int getCharType(char *symbol, int *addC); 
int getInput(char **str);
int checkString(char str[], size_t length);
void * checkRealloc(void *pointer, size_t newLength);
int main(int argc, char const *argv[])
{
    while (1){
        size_t len = 0;
        char *string;
        len = getInput(&string);
        if (isFinalState[checkString(string, len)]){
            if (*string == '$') printf("It is a Pascal hex num \n");
            else printf("It is a C hex num \n");
        }
        else {
            printf("It is not a hex num in C or Pascal \n");
        }
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
        if (length > capacity - 2)
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
    return state;
}

void findCandPascalNums(char *str){
    int i = 0, *addC, counterNums = 0, state = 1, typeOfChar, indexStartNum, isC;
    char *cStrings, *pascalStrings; //надо память выделять под них и потом записывать туда соотв строки
    while (str[i] != '\0'){
        typeOfChar = getCharType(str + i + *addC, addC);
        state = values[state][typeOfChar];
        if (isFinalState[state]){
            isC = *addC;
            counterNums++;
        }
        if ((state == 0) && (typeOfChar == Unknown)){
            state = 1;
            i++;
            continue;
        }
        if (typeOfChar = StartN) indexStartNum = i;
        i++;
    }
}