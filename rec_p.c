#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

int sp_ret;

void printStack(int sp, int *stack, bool nl) {
    printf(" [ ");
    for(int x=0; x < sp; x ++) {
        printf("%d ", stack[x]);
    }
    printf("] ");
    if (nl) {
        printf("\n");
    }
}

int skip_bkt(int pc, char *prog, char up, char down, int step) {
    int offset = pc + step;
    int level = 1;

    while (prog[offset] && level > 0) {
        level += prog[offset] == up;
        level -= prog[offset] == down;
        offset += step;
    }

    return offset;
}

void run(char *prog, register int sp, int *stack) {
    int x, v;

    for(int pc = 0; prog[pc]; pc++) {
        sp = (sp < 0) ? 0 : sp;

        switch(prog[pc]) {
            case '0': case '1': case '2':
            case '3': case '4': case '5':
            case '6': case '7': case '8':
            case '9':
                if(isdigit(prog[pc-1]))
                    stack[sp-1] = stack[sp-1] * 10 + prog[pc] - 48;
                else
                    stack[sp++] = prog[pc] - 48;
                break;

            case '+':
                stack[sp-2] += stack[sp-1];
                sp --;
                break;

            case '*':
                stack[sp-2] *= stack[sp-1];
                sp --;
                break;
            
            case '_':
                stack[sp-1] = - stack[sp-1];
                break;

            case '=':
                stack[sp-1] = stack[sp-1] == 0;
                break;

            case ':':
                x = stack[sp-1];
                stack[sp-1] = stack[sp-x-2];
                break;

            case ';':
                x = stack[sp-1];
                v = stack[sp-2];
                sp -= 2;
                stack[sp-x-1] = v;
                break;

            case '^':
                if (stack[--sp] != 0)
                    pc = skip_bkt(pc, prog, '[', ']', 1) - 1;
                break;

            case ']':
                pc = skip_bkt(pc, prog, ']', '[', -1);
                break;

            // Debugging commands
            case 'P':
                printf("%d\n", stack[--sp]);
                break;

            case 'R':
                scanf("%d", &v);
                stack[sp++] = v;
                break;

            case 'p':
                putchar(stack[--sp]);
                break;

            case 'r':
                v = getchar();
                stack[sp++] = v;
                break;

            case 's':
                printStack(sp, stack, true);
                break;
        }
    }
    
    // For REPL (since local values are lost across function calls)
    sp_ret = sp;
}

int main(int argc, char **argv) {
    // *stack = pointer to bottom of stack
    int *stack = (int*)malloc(sizeof(int) * 20000);
    int sp = 0;

    if (argc < 2) {
        char *s = (char*) malloc(sizeof(char) * 100);
        unsigned long len = 0;
        
        for(;;) {
            printf("r> ");
            len = getline(&s, &len, stdin);
            
            if(len == -1) {
                free(s);
                break;
            }

            run(s, sp, stack);
            sp = sp_ret;

            printStack(sp, stack, true);
        }
    } else {
        FILE *fp = fopen(argv[1], "r");
        char prog[2000] = {};

        fread(&prog, sizeof(char), -1, fp);
        fclose(fp);

        run(prog, sp, stack);
    }

    free(stack);
}
