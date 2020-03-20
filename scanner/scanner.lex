/**
 * scanner.flex
 * Description: TODO
 * Author: Benjamin Zinschitz (00940280)
 * Usage: TODO
 */
        int num_chars = 0;
        int num_lines = 0;
        int num_column = 0;

%%

\n      ++num_chars; ++num_lines; num_column = 0;
.       ++num_chars; ++num_column;

%%

int main(int argc, char *argv[]) {
    yylex();
    printf("Scanned %s\nlines:\t%d\nchars:\t%d\n", argc < 2 ? "stdin" : argv[1], num_lines, num_chars);
}
