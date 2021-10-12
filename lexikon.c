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

char keywords[15][10] = {"do", "else", "end", "function", "global", "if", "integer", "local", "nil", "number", "require", "return", "string", "then", "while"};

void printToken(tToken *token){
    printf("token attribute: %s\n", token->attribute);
    printf("token type: %s\n", token->type);
    printf("token line: %d\n", token->line);
    printf("\n");   
}

void initToken(tToken *token){
    token->line = 1;
    token->type = " ";
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
    token->attribute = malloc(100);

    token->type = malloc(11);
    if(token->type == NULL){
        fprintf(stderr, "ERROR: Failed malloc");
        return 1;
    }
    initToken(token); 

    bool KeyWord = false;
    bool DoubleNumber = false;
    bool Comment = false;
    //přidat, oddělat states
    //DODĚLAT BLOKOVÉ KOMENTÁŘE
    //PŘEPSAT NA DYNAMICKOU PAMĚŤ
    //

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
     */
    char states[9] = {'s', '1', '2', '3', '4', '5', '6', '7', '8'};
    char state = states[0];

    //switch: stavy
    //zkusit popřemýšlet
    while ( c != EOF)
    {
        c = getc(stdin);
        switch (state){
        case 's':
            // ID/KeyWord
            if ((c >= 97 && c <= 122) || c == '_' || (c >= 65 && c <= 90))
            {
                state = states[1];
                token->attribute[i] = c;
                i++;
            }
            //Integer and Double
            else if((c >= 48 && c <= 57)){
                state = states[2];
                token->attribute[i] = c;
                i++;
            }
            //assign operator
            else if(c == ':'){
                token->attribute[i] = c;
                ++i;
                token->attribute[i] = '\0';
                token->type = "DataAssign";
                i = 0;
                
                state = states[0];
                printToken(token);

            }
            //Add operator
            else if(c == '+'){
                token->attribute[i] = c;
                ++i;
                token->attribute[i] = '\0';
                token->type = "add";
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
                token->attribute[i] = '\0';
                token->type = "divide";
                i = 0;
                printToken(token);
            }
            else if(c == '*'){
                token->attribute[i] = c;
                ++i;
                token->attribute[i] = '\0';
                token->type = "mul";
                i = 0;
                printToken(token);
            }
            else if(c == '"'){
                token->attribute[i] = c;
                state = states[4];
                i++;
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
            else if(c == '\n'){
                token->line++;
            }
            else if(c == ' '){
                continue;
            }
            else{
                fprintf(stderr, "ERROR: Invalid character \"%c\" on line %d\n",c ,token->line);
                return 1;
            }
            break;

        case '1':
            if ((c >= 97 && c <= 122) || c == '_' || (c >= 48 && c <= 57)|| (c >= 65 && c <= 90)){
                token->attribute[i] = c;
                i++;
            }
            else{
                ungetc(c, stdin);
                token->attribute[i] = '\0';
                KeyWord = IsKeyWord(token);
                if (KeyWord)
                {
                    token->type = "keyword";
                    KeyWord = false;
                    i = 0;
                }
                else{
                    token->type = "id";
                    i = 0;
                }
                
                state = states[0];
                printToken(token);
            }
        break;

        case '2':
            if ((c >= 48 && c <= 57)){
                token->attribute[i] = c;
                i++;
            }
            else if(c == 'E' || c == 'e' || c == '+' || c == '-' || c == '.'){
                token->attribute[i] = c;
                i++;
                DoubleNumber = true;
            }
            else{
                ungetc(c, stdin);
                token->attribute[i] = '\0';
                if (DoubleNumber){
                    token->type = "double";
                    DoubleNumber = false;
                }
                else{
                    token->type = "integer";
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
                token->type = "equals";
                i = 0;
                
                state = states[0];
                printToken(token);
            }
            else{
                ungetc(c, stdin);
                token->attribute[i] = '\0';
                token->type = "assign";
                i = 0;
                
                state = states[0];
                printToken(token);
            }
            
            
        break;
        
        case '4':
            token->attribute[i] = c;
            i++;
            if (c == '"')
            {
                token->attribute[i] = '\0';
                token->type = "string";
                i = 0;
                
                state = states[0];
                printToken(token);
            }
            
        break;
        //KONTROVERZNÍ->PRAVDĚPODOBNĚ SMAZAT
        case '5':
            if(c == '-'){
                //type = comment;
                //token->type = type;
                token->attribute[i] = c;
                Comment = true;
                i++;
            }
            else if(Comment){
                
                if(c == '\n'){
                    ungetc(c, stdin);
                    token->attribute[i] = '\0';
                    i = 0;
                    state = states[0];
                    printToken(token);
                    Comment = false;
                }
                token->attribute[i] = c;
                i++;
            }
            else{
                ungetc(c, stdin);
                token->attribute[i] = '\0';
                token->type = "sub";
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
                token->type = "LEQT";
                i = 0;
                
                state = states[0];
                printToken(token);
            }
            else{
                ungetc(c, stdin);
                token->attribute[i] = '\0';
                token->type = "LT";
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
                token->type = "MEQT";
                i = 0;
                
                state = states[0];
                printToken(token);
            }
            else{
                ungetc(c, stdin);
                token->attribute[i] = '\0';
                token->type = "MT";
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
                token->type = "NEQ";
                i = 0;
                
                state = states[0];
                printToken(token);
            }
            else{
                fprintf(stderr, "ERROR: Missing \'=\' on %d\n", token->line);
                return 1;
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
