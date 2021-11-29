#include <stdio.h>

int main(int argc, char *argv[])
{
    int a, b;
    char op;
    FILE* f = fopen(argv[1], "r");
    fscanf(f, "%d%c%d\n", &a, &op, &b);
    printf("%d%c%d\n",a,op,b);
    fclose(f);


//    scanf("%d%c%d", &a, &op, &b);
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
