#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<ctype.h>

typedef struct Token{
    int type;
    char attribute[100];
    int line;
}tToken;

enum Type {id, integer, keyword, add, sub, mul, divide, string, _double, assignment}type;
char keywords[15][10] = {"do", "else", "end", "function", "global", "if", "integer", "local", "nil", "number", "require", "return", "string", "then", "while"};

void printToken(tToken *token){
    printf("token attribute: %s\n", token->attribute);
    printf("token type: %d\n", token->type);
    printf("token line: %d\n", token->line);
    printf("\n");   
}

void initToken(tToken *token){
    token->line = 1;
    token->type = -1;
}

void deleteToken(tToken *token){
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

//smazat????
bool IsNumber(tToken *token){
    int length = strlen(token->attribute);
    for (int i = 0; i < length; i++)
    {
        if (!(token->attribute[i] >= 48 && token->attribute[i] <= 57))
        {
            return false;
        }
    }
    return true;
}

//smazat???
void Finish(tToken *token, bool KeyWord, bool Number){
    KeyWord = IsKeyWord(token);
    Number = IsNumber(token);
    if (KeyWord){
        type = keyword;
        token->type = type;
        KeyWord = false;
    }
    else if(Number){
        type = integer;
        token->type = type;
        Number = false;
    }
    else{
        if ((token->attribute[0] >= 48 && token->attribute[0] <= 57) || token->attribute[0] == '_')
        {
            fprintf(stderr, "line %d: ERROR: Wrong format od an ID\n", token->line);
        }
        type = id;
        token->type = type;
    }
}

int main(int argc, char const *argv[])
{   
    char c = ' ';
    int i = 0;
    tToken *token = malloc(sizeof(struct Token));
    initToken(token); 

    bool KeyWord = false;
    //bool Number = false;
    //přidat, oddělat states
    char states[11] = {'s', '1', '2', '3', '4', '5', '6', '7', '8', 'c', 'q'};
    char state = states[0];

    //switch: stavy
    //zkusit popřemýšlet
    while (c != '\n' && c != EOF)
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
            else if((c >= 48 && c <= 57)){
                state = states[2];
                token->attribute[i] = c;
                i++;
            }
            else if(c == ':'){
                state = states[10];
                token->attribute[i] = c;
                i++;

            }
            else if(c == '+'){
                //addition
            }
            else if(c == '-'){
                //sub
            }
            else if(c == '/'){
                //div
            }
            else if(c == '*'){
                //mul
            }
            else{
                
            }
            //cases na <,>,<=,>=,~=
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
                    type = keyword;
                    token->type = type;
                    KeyWord = false;
                    i = 0;
                }
                else{
                    type = id;
                    token->type = type;
                    i = 0;
                }
                
                state = states[0];
                printToken(token);
                continue;
            }
        break;

        case '2':
            if ((c >= 48 && c <= 57)){
                token->attribute[i] = c;
                i++;
            }
            else{
                ungetc(c, stdin);
                token->attribute[i] = '\0';
                type = integer;
                token->type = type;
                i = 0;
                
                state = states[0];
                printToken(token);
            }
            continue;
        break;
        
        case 'q':
            token->attribute[i] = c;
            ++i;
            token->attribute[i] = '\0';
            //PŘIŘAZENÍ:type = integer;
            token->type = type;
            i = 0;
                
            state = states[0];
        break;
        //CASES PRO <,>,<=,>=,<>
        
        //asi blbost 
        //  |
        //  v
        case 'p':
            if(c == '}'){
                state = states[0];
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
