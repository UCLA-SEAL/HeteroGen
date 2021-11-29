#include <stdio.h>

int main(int argc, char *argv[])
{
    int a, b, c, d;
    a = atoi(argv[1])
    char op;
    FILE* f = fopen(argv[1], "r");
    fscanf(f, "%d%c%d\n", &a, &op, &b);
    printf("%d%c%d\n",a,op,b);
    fclose(f);

    int result;
    switch (op) {
        case '+':
            result = a + b;
            break;
        case '-':
            result = a - b;
            break;
        case '*':
            result = a * b;
            break;
        case '/':
            result = a / b;
            break;
        default:
            return 1;
    }
    printf("%d\n", result);
    return 0;
}
