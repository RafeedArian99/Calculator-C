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

/**
 * Operation Priority Order:
 * 0 - Add/Sub
 * 1 - Mul/Div
 * 2 - Exp
 * 3 - Parenthesis
 * @param start
 * @param end
 * @param opOrder
 * @return
 */
double evalExpression(int start, int end, int opOrder) {
    int addSubLoc = -1; // Location of addition/subtraction.
    int invMulLoc = -1; // Location of operation-less multiplication. Example: 2(5+3).
    int mulDivLoc = -1; // Location of multiplication/division.
    int expLoc = -1;    // Location of exponent.
    int ignorable = 0;  // Flag for skipping over parenthetical bodies.

    // Find location of operations. Break if lowest priority operation is found.
    for (int i = start; i < end && addSubLoc == -1; i++) {
        if (*(expression + i) == '(') {
            ignorable++;
            if (i > start && !isOperator(*(expression + i - 1)) && invMulLoc == -1)
                invMulLoc = i;
        } else if (*(expression + i) == ')')
            ignorable--;

        else if (!ignorable) {
            if (opOrder < 1 && *(expression + i) == '+' || (i > start && *(expression + i) == '-'))
                addSubLoc = i;
            else if (opOrder < 2 && mulDivLoc == -1 && (*(expression + i) == '*' || *(expression + i) == '/'))
                mulDivLoc = i;
            else if (opOrder < 3 && expLoc == -1 && *(expression + i) == '^')
                expLoc = i;
        }
    }

    // Split expression into [left expression] [OP] [right expression] and evaluate the left and right expressions.
    // Operations in ascending order of priority top-to-bottom.
    if (addSubLoc != -1) {
        if (*(expression + addSubLoc) == '+')
            return evalExpression(start, addSubLoc, 1) + evalExpression(addSubLoc + 1, end, 0);
        else
            return evalExpression(start, addSubLoc, 1) + evalExpression(addSubLoc, end, 0);
    } else if (invMulLoc != -1) {
        return evalExpression(start, invMulLoc, 2) * evalExpression(invMulLoc, end, 1);
    }
    else if (mulDivLoc != -1) {
        if (*(expression + mulDivLoc) == '*')
            return evalExpression(start, mulDivLoc, 2) * evalExpression(mulDivLoc + 1, end, 1);
        else
            return evalExpression(start, mulDivLoc, 2) / evalExpression(mulDivLoc + 1, end, 1);
    } else if (expLoc != -1) {
        return pow(evalExpression(start, expLoc, 3), evalExpression(expLoc + 1, end, 2));
    } else if (*(expression + start) == '(' && *(expression + end - 1) == ')') {
        return evalExpression(start + 1, end - 1, 0);
    }

    // Convert operation-less expression from string to double.
    return convertToDouble(start, end);
}

int main() {
    expSize = malloc(4);
    expression = malloc(1);

    printf("Calculate:");
    getLine(&expression, expSize);

    printf("%s = %lf", expression, evalExpression(0, *expSize, 0));
}
