#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<ctype.h>

typedef struct Token{
    char *type;
    char *attribute;
    int line;
}tToken;

#define TOKEN_LENGTH 100
#define REALL_TOKEN_LEN 100
char keywords[15][10] = {"do", "else", "end", "function", "global", "if", "integer", "local", "nil", "number", "require", "return", "string", "then", "while"};

void printToken(tToken *token){
    printf("token attribute: %s\n", token->attribute);
    printf("token type: %s\n", token->type);
    printf("token line: %d\n", token->line);
    printf("\n");   
}

void initToken(tToken *token){
    token->line = 1;
}

void deleteToken(tToken *token){
    free(token->attribute);
    free(token->type);
    free(token);
}

bool IsKeyWord(tToken *token){
    for (int i = 0; i < 15; i++)
    {
        if (strcmp(keywords[i], token->attribute) == 0)
        {
            return true;
        }
    }
    return false;
}

int main(int argc, char const *argv[])
{   
    char c = ' ';
    int i = 0;
    tToken *token = malloc(sizeof(struct Token));
    if(token == NULL){
        fprintf(stderr, "ERROR: Failed malloc");
        return 1;
    }
    int length = TOKEN_LENGTH;
    token->attribute = malloc(TOKEN_LENGTH * sizeof(char));
    if(token->attribute == NULL){
        fprintf(stderr, "ERROR: Failed malloc");
        return 1;
    }

    token->type = malloc(12 * sizeof(char));
    if(token->type == NULL){
        fprintf(stderr, "ERROR: Failed malloc");
        return 1;
    }
    initToken(token); 

    bool KeyWord = false;
    bool DoubleNumber = false;
    bool Comment = false;
    
    //KOMENTÁŘE

    /**SEZNAM TYPŮ:
     * id
     * integer
     * double
     * DataAssign
     * add
     * divide
     * mul
     * keyword
     * equals
     * assign
     * string
     * sub
     * LEQT
     * LT
     * MEQT
     * MT
     * NEQ
     * dot 
     * IntegerDiv
     * concatenate
     * LeftBracket
     * RightBracket
     * LeftCurBr
     * RightCurBr
     * length
     * comma
     */

    /**
     * s = startovní stav
     * 1 = ID
     * 2 = Integer
     * 3 = Rovná se
     * 4 = string
     * 5 = comment
     * 6 = LET
     * 7 = MET
     * 8 = NE
     * 9 = konkatenace
     * d = dělení
     */
    char states[11] = {'s', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'd'};
    char state = states[0];

    while ( c != EOF)
    {
        c = getc(stdin);
        switch (state){
        case 's':
            // ID/KeyWord
            if ((c >= 'a' && c <= 'z') || c == '_' || (c >= 'A' && c <= 'Z'))
            {
                state = states[1];
                token->attribute[i] = c;
                i++;
            }
            //Integer and Double
            else if((c >= '0' && c <= '9')){
                state = states[2];
                token->attribute[i] = c;
                i++;
            }
            //assign operator
            else if(c == ':'){
                token->attribute[i] = c;
                ++i;
                token->attribute[i] = '\0';
                strcpy(token->type, "DataAssign");
                i = 0;
                
                state = states[0];
                printToken(token);

            }
            //Add operator
            else if(c == '+'){
                token->attribute[i] = c;
                ++i;
                token->attribute[i] = '\0';
                strcpy(token->type, "add");
                i = 0;
                printToken(token);

            }
            else if(c == '='){
                state = states[3];
                token->attribute[i] = c;
                i++;
            }
            else if(c == '-'){
                token->attribute[i] = c;
                ++i;
                state = states[5];
            }
            else if(c == '/'){
                token->attribute[i] = c;
                ++i;
                state = states[10];
            }
            else if(c == '*'){
                token->attribute[i] = c;
                ++i;
                token->attribute[i] = '\0';
                strcpy(token->type, "mul");
                i = 0;
                printToken(token);
            }
            else if(c == '"'){
                state = states[4];
            }
            //cases na <,>,<=,>=,~=
            else if(c == '<'){
                state = states[6];
                token->attribute[i] = c;
                i++;
            }
            else if(c == '>'){
                state = states[7];
                token->attribute[i] = c;
                i++;
            }
            else if(c == '~'){
                state = states[8];
                token->attribute[i] = c;
                i++;
            }
            else if(c == '('){
                token->attribute[i] = c;
                ++i;
                token->attribute[i] = '\0';
                strcpy(token->type, "LeftBracket");
                i = 0;
                printToken(token);
            }
            else if(c == ')'){
                token->attribute[i] = c;
                ++i;
                token->attribute[i] = '\0';
                strcpy(token->type, "RightBracket");
                i = 0;
                printToken(token);
            }
            else if(c == ','){
                token->attribute[i] = c;
                ++i;
                token->attribute[i] = '\0';
                strcpy(token->type, "comma");
                i = 0;
                printToken(token);
            }
            else if(c == '.'){//dalsi faze
                state = states[9];
                token->attribute[i] = c;
                i++;
            }
            else if(c == '{'){
                token->attribute[i] = c;
                ++i;
                token->attribute[i] = '\0';
                strcpy(token->type, "LeftCurBr");
                i = 0;
                printToken(token);
            }
            else if(c == '}'){
                token->attribute[i] = c;
                ++i;
                token->attribute[i] = '\0';
                strcpy(token->type, "RightCurBr");
                i = 0;
                printToken(token);
            }
            else if(c == '#'){
                token->attribute[i] = c;
                ++i;
                token->attribute[i] = '\0';
                strcpy(token->type, "length");
                i = 0;
                printToken(token);
            }
            else if(c == '\n'){
                token->line++;
            }
            else if(c == ' '){
                continue;
            }
            else{
                if(c!=EOF){
                    fprintf(stderr, "ERROR: Invalid character \"%c\" on line %d\n",c ,token->line);
                    return 1;
                }
            }
        break;

        case '1':
            if ((c >= 'a' && c <= 'z') || c == '_' || (c >= '0' && c <= '9')|| (c >= 'A' && c <= 'Z')){
                token->attribute[i] = c;
                i++;
                
                if (i > (length - 1))
                {
                    length += REALL_TOKEN_LEN;
                    token->attribute = realloc(token->attribute, length * sizeof(char));
                }
                
            }
            else{
                ungetc(c, stdin);
                token->attribute[i] = '\0';
                KeyWord = IsKeyWord(token);
                if (KeyWord)
                {
                    strcpy(token->type, "keyword");
                    KeyWord = false;
                    i = 0;
                }
                else{
                    strcpy(token->type, "id");
                    i = 0;
                }
                
                state = states[0];
                printToken(token);
            }
        break;

        case '2':
            if ((c >= '0' && c <= '9')){
                token->attribute[i] = c;
                i++;
                if (i > (length - 1))
                {
                    length += REALL_TOKEN_LEN;
                    token->attribute = realloc(token->attribute, length * sizeof(char));
                }
            }
            else if(c == 'E' || c == 'e' || c == '+' || c == '-' || c == '.'){
                token->attribute[i] = c;
                i++;
                if (i > (length - 1))
                {
                    length += REALL_TOKEN_LEN;
                    token->attribute = realloc(token->attribute, length * sizeof(char));
                }
                DoubleNumber = true;
            }
            else{
                ungetc(c, stdin);
                token->attribute[i] = '\0';
                if (DoubleNumber){
                    strcpy(token->type, "double");
                    DoubleNumber = false;
                }
                else{
                    strcpy(token->type, "integer");
                }
                
                
                i = 0;
                
                state = states[0];
                printToken(token);
            }
        break;
        
        case '3':
            if (c == '='){
                token->attribute[i] = c;
                ++i;
                token->attribute[i] = '\0';
                strcpy(token->type, "equals");
                i = 0;
                
                state = states[0];
                printToken(token);
            }
            else{
                ungetc(c, stdin);
                token->attribute[i] = '\0';
                strcpy(token->type, "assign");
                i = 0;
                
                state = states[0];
                printToken(token);
            }
            
            
        break;
        
        case '4':
            token->attribute[i] = c;
            i++;
            if (i > (length - 1))
            {
                length += REALL_TOKEN_LEN;
                token->attribute = realloc(token->attribute, length * sizeof(char));
            }
            if(c == '\\'){
                c = getc(stdin);
                --i;
                if(c == '"'){
                    token->attribute[i] = c;
                    i++;
                }
                else if(c == 't'){
                    token->attribute[i] = '\t';
                    i++;
                }
                else if(c == 'n'){
                    token->attribute[i] = '\n';
                    i++;
                }
                else if(c == '\\'){
                    token->attribute[i] = '\\';
                    i++;
                }
                else if(c >= '0' && c <='2'){
                    char escaped[4];
                    escaped[0] = c;
                    c = getc(stdin);
                    if (c >= '0' && c <='9'){
                        escaped[1] = c;
                        c = getc(stdin);
                        if (c >= '0' && c <='9'){
                            escaped[2] = c;
                            escaped[3] = '\0';
                            int aux = atoi(escaped);
                            if(aux >= 1 && aux <= 255){
                                token->attribute[i] = aux;
                                i++;
                            }
                            else{
                                fprintf(stderr, "ERROR: Wrong format of escape expression on line %d!\n", token->line);
                                return 1;
                            }
                        }
                        else{
                            fprintf(stderr, "ERROR: Wrong format of escape expression on line %d!\n", token->line);
                            return 1;
                        }
                    }
                    else{
                        fprintf(stderr, "ERROR: Wrong format of escape expression on line %d!\n", token->line);
                        return 1;
                    }
                }
                else{
                    fprintf(stderr, "ERROR: Wrong format of escape expression on line %d!\n", token->line);
                    return 1;
                }
            }
            else if (c == '"')
            {
                i--;
                token->attribute[i] = '\0';
                strcpy(token->type, "string");
                i = 0;
                
                state = states[0];
                printToken(token);
            }
            else if(c == '\n' || c == EOF){
                fprintf(stderr, "ERROR: Unexpected End of file on line %d!", token->line);
                return 1;
            }
            
        break;

        case '5':
            if(c == '-'){
                Comment = true;
                i = 0;
            }
            else if (Comment && c == '\n' ){
                state = states[0];
                token->line++;
                Comment = false;
            }
            
            else if(c == '[' && (c = getc(stdin) == '[')){

                do
                {
                    c = getc(stdin);
                    if(c == EOF){
                        fprintf(stderr, "ERROR: Unterminated comment on line %d!\n", token->line);
                        return 1;
                    }
                    if(c == ']'){
                        if ((c = getc(stdin) == ']'))
                        {
                            state = states[0];
                            break;
                        }
                        ungetc(c, stdin);
                    }
                    if (c == '\n')
                    {
                        token->line++;
                    }
                }while (42); 
                
            }
            else if(!Comment){
                ungetc(c, stdin);
                token->attribute[i] = '\0';
                strcpy(token->type, "sub");
                i = 0;
                state = states[0];
                printToken(token);
            }
            
        break;

        case '6':
            if(c == '='){
                token->attribute[i] = c;
                ++i;
                token->attribute[i] = '\0';
                strcpy(token->type, "LEQT");
                i = 0;
                
                state = states[0];
                printToken(token);
            }
            else{
                ungetc(c, stdin);
                token->attribute[i] = '\0';
                strcpy(token->type, "LT");
                i = 0;
                
                state = states[0];
                printToken(token);
            }
        break;
        case '7':
            if(c == '='){
                token->attribute[i] = c;
                ++i;
                token->attribute[i] = '\0';
                strcpy(token->type, "MEQT");
                i = 0;
                
                state = states[0];
                printToken(token);
            }
            else{
                ungetc(c, stdin);
                token->attribute[i] = '\0';
                strcpy(token->type, "MT");
                i = 0;
                
                state = states[0];
                printToken(token);
            }
        break;
        case '8':
            if(c == '='){
                token->attribute[i] = c;
                ++i;
                token->attribute[i] = '\0';
                strcpy(token->type, "NEQ");
                i = 0;
                
                state = states[0];
                printToken(token);
            }
            else{
                fprintf(stderr, "ERROR: Missing \'=\' on %d\n", token->line);
                return 1;
            }

        break;

        case '9':
            if(c == '.'){
                token->attribute[i] = c;
                ++i;
                token->attribute[i] = '\0';
                strcpy(token->type, "concatenate");
                i = 0;
                
                state = states[0];
                printToken(token);
            }
            else{
                ungetc(c, stdin);
                token->attribute[i] = '\0';
                strcpy(token->type, "dot");
                i = 0;
                
                state = states[0];
                printToken(token);
            }
        break;

        case 'd':
            if(c == '/'){
                token->attribute[i] = c;
                ++i;
                token->attribute[i] = '\0';
                strcpy(token->type, "IntegerDiv");
                i = 0;
                
                state = states[0];
                printToken(token);
            }
            else{
                ungetc(c, stdin);
                token->attribute[i] = '\0';
                strcpy(token->type, "div");
                i = 0;
                
                state = states[0];
                printToken(token);
            }
        break;

        default:
            
            break;
        }
        
    }
    (void)argc;
    (void)argv;
    deleteToken(token);
    return 0;
}