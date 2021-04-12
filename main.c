#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int* expSize;
char* expression;

void getLine(char** line, int* size)
{
    char c;
    int i = 0;

    while (1) {
        c = getc(stdin);
        if (c == EOF || c == '\n')
            break;

        *line = realloc(*line, i + 2);
        *(*line + i++) = c;
    }

    *(*line + i) = '\0';
    *size = i;
}

double convertToDouble(int start, int end) {
    double num = 0.0;
    int neg = *(expression + start) == '-';
    for (int i = neg ? start + 1 : start; i < end; i++)
        num = num * 10 + *(expression + i) - '0';

    return neg? -num : num;
}

void printSubExpression(int start, int end) {
    for (int i = start; i < end; i++)
        printf("%c", *(expression + i));
}

double evalExpression(int start, int end) {
    if (start == end)
        return 0;
    /**/ double returnStatement;

    int expLoc = -1, mulDivLoc = -1, addsubLoc = -1;
    int ignorable = 0;

    for (int i = start; i < end && addsubLoc == -1; i++) {
        if (*(expression + i) == '(')
            ignorable++;
        else if (*(expression + i) == ')')
            ignorable--;
        else if (!ignorable) {
            if (*(expression + i) == '+' || *(expression + i) == '-')
                addsubLoc = i;
            else if (mulDivLoc == -1 && (*(expression + i) == '*' || *(expression + i) == '/'))
                mulDivLoc = i;
            else if (expLoc == -1 && *(expression + i) == '^')
                expLoc = i;
        }
    }

    if (addsubLoc != -1) {
        if (*(expression + addsubLoc) == '+')
            returnStatement = evalExpression(start, addsubLoc) + evalExpression(addsubLoc + 1, end);   // ADD
        else
            returnStatement = evalExpression(start, addsubLoc) - evalExpression(addsubLoc + 1, end);   // SUBTRACT
    } else if (mulDivLoc != -1) {
        if (*(expression + mulDivLoc) == '*')
            returnStatement = evalExpression(start, mulDivLoc) * evalExpression(mulDivLoc + 1, end);   // MULTIPLY
        else
            returnStatement = evalExpression(start, mulDivLoc) / evalExpression(mulDivLoc + 1, end);   // DIVIDE
    }

    else if (expLoc != -1) {
        returnStatement = pow(evalExpression(start, expLoc), evalExpression(expLoc + 1, end));      // POWER
    } else if (*(expression + start) == '(' && *(expression + end - 1) == ')') {
        returnStatement = evalExpression(start + 1, end - 1);                                      // PARENTHESIS
    } else {
        returnStatement = convertToDouble(start, end);                                                      // NUMBER
    }

    printSubExpression(start, end);
    printf(" = %lf\n", returnStatement);
    return returnStatement;
}

int main() {
    expSize = malloc(4);
    expression = malloc(1);
    getLine(&expression, expSize);

    printf("%lf", evalExpression(0, *expSize));
}
