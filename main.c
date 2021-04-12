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
        c = (char) getc(stdin);
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

int isOperator(char i) {
    return i == '+' || i == '-' || i == '*' || i == '/' || i == '^' || i == '(';
}

double evalExpression(int start, int end) {
    int addSubLoc = -1; // Location of '+' or noninitial '-'.
    int invMulLoc = -1; // Location of "invisible '*'". Example: 2(5+3).
    int mulDivLoc = -1; // Location of '*' or '/'.
    int expLoc = -1;    // Location of '^'.
    int ignorable = 0;  // Flag for skipping over parenthetical bodies.

    /* Find location of operations. Break if lowest priority operation is found. */
    for (int i = start; i < end && addSubLoc == -1; i++) {
        if (*(expression + i) == '(') {
            ignorable++;
            if (i > start && !isOperator(*(expression + i - 1)) && invMulLoc == -1)
                invMulLoc = i;
        } else if (*(expression + i) == ')')
            ignorable--;

        else if (!ignorable) {
            if (*(expression + i) == '+' || (*(expression + i) == '-' && i > start))
                addSubLoc = i;
            else if (mulDivLoc == -1 && (*(expression + i) == '*' || *(expression + i) == '/'))
                mulDivLoc = i;
            else if (expLoc == -1 && *(expression + i) == '^')
                expLoc = i;
        }
    }

    /* Split expression and apply the correct operations. Operations in ascending order of priority. */
    if (addSubLoc != -1) {
        if (*(expression + addSubLoc) == '+')
            return evalExpression(start, addSubLoc) + evalExpression(addSubLoc + 1, end);
        else
            return evalExpression(start, addSubLoc) + evalExpression(addSubLoc, end);
    } else if (invMulLoc != -1) {
        return evalExpression(start, invMulLoc) * evalExpression(invMulLoc, end);
    }
    else if (mulDivLoc != -1) {
        if (*(expression + mulDivLoc) == '*')
            return evalExpression(start, mulDivLoc) * evalExpression(mulDivLoc + 1, end);
        else
            return evalExpression(start, mulDivLoc) / evalExpression(mulDivLoc + 1, end);
    } else if (expLoc != -1) {
        return pow(evalExpression(start, expLoc), evalExpression(expLoc + 1, end));
    } else if (*(expression + start) == '(' && *(expression + end - 1) == ')') {
        return evalExpression(start + 1, end - 1);
    }

    /* Convert operation-less expression from string to double. */
    return convertToDouble(start, end);
}

int main() {
    expSize = malloc(4);
    expression = malloc(1);
    getLine(&expression, expSize);

    printf("%lf", evalExpression(0, *expSize));
}
