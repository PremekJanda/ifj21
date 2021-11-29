/**
 *  Soubor: lexikon.c
 * 
 *  Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
 *  Poslední změna:	26. 11. 2021 09:09:57
 *  Autoři: David Kocman  - xkocma08, VUT FIT
 *          Radomír Bábek - xbabek02, VUT FIT
 *          Martin Ohnút  - xohnut01, VUT FIT
 *          Přemek Janda  - xjanda28, VUT FIT
 *  Popis: Obsahuje implementaci lexikální analýzy (scanner) překladače
 */
#include "lexikon.h"

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
char keywords[15][10] = {"do", "else", "end", "function", "global", "if", "integer", "local", "nil", "number", "require", "return", "string", "then", "while"};

char c;
int i;

bool KeyWord;
bool DoubleNumber;
bool Comment;

int scanner(tToken *token){
    c = ' ';
    i = 0;

    KeyWord = false;
    DoubleNumber = false;
    Comment = false;

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
     * modulo
     * power
     */
    
    char state = states[0];
    int length = TOKEN_LENGTH;

    while (c  != EOF)
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
            //Integer a Double
            else if((c >= '0' && c <= '9')){
                state = states[2];
                token->attribute[i] = c;
                i++;
            }
            //assign operator
            else if(c == ':'){
                STokenFinish(token, &i, "DataAssign", c);
                return 0;
            }
            //Add operator
            else if(c == '+'){
                STokenFinish(token, &i, "add", c);
                return 0;

            }
            //rovná se/přiřazení
            else if(c == '='){
                state = states[3];
                token->attribute[i] = c;
                i++;
            }
            //mínus/komentáře
            else if(c == '-'){
                token->attribute[i] = c;
                ++i;
                state = states[5];
            }
            //dělení a celočíselné dělení
            else if(c == '/'){
                token->attribute[i] = c;
                ++i;
                state = states[10];
            }
            //krát
            else if(c == '*'){
                STokenFinish(token, &i, "mul", c);
                return 0;
            }
            //string
            else if(c == '"'){
                token->attribute[i] = c;
                i++;
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
            //závorky
            else if(c == '('){
                STokenFinish(token, &i, "(", c);
                return 0;
            }
            else if(c == ')'){
                STokenFinish(token, &i, ")", c);
                return 0;
            }
            //čárka
            else if(c == ','){
                STokenFinish(token, &i, "comma", c);
                return 0;
            }
            //tečka/konkatenace
            else if(c == '.'){//dalsi faze
                state = states[9];
                token->attribute[i] = c;
                i++;
            }
            //křivé závorky
            else if(c == '{'){
                STokenFinish(token, &i, "LeftCurBr", c);
                return 0;
            }
            else if(c == '}'){
                STokenFinish(token, &i, "RightCurBr", c);
                return 0;
            }
            //délka řetězce
            else if(c == '#'){
                STokenFinish(token, &i, "length", c);
                return 0;
            }
            //cases  na % a ^
            else if(c == '%'){
                STokenFinish(token, &i, "modulo", c);
                return 0;
            }
            else if(c == '^'){
                STokenFinish(token, &i, "power", c);
                return 0;
            }
            //inkrementace počítadla řádku
            else if(c == '\n'){
                token->line++;
            }
            //ignorace bílého znaku
            else if(c == ' ' || c == '\r' || c == '\t'){
                continue;
            }
            else if(c == EOF){
                token->attribute[0] = '\0';
                strcpy(token->type, "EOF");
                return 0;
            }
            else{
                //ošetření špatného charakteru
                fprintf(stderr, "ERROR: Invalid character \"%c\" on line %d\n",c ,token->line);
                return 1;
            }
        break;

        //stav vytvoření id/keyword tokenu
        case '1':
            if ((c >= 'a' && c <= 'z') || c == '_' || (c >= '0' && c <= '9')|| (c >= 'A' && c <= 'Z')){
                token->attribute[i] = c;
                i++;
                //realloc při nedostatečné délce tokenu
                if (i > (length - 1))
                {
                    length += REALL_TOKEN_LEN;
                    token->attribute = realloc(token->attribute, length * sizeof(char));
                }
                
            }
            else{
                //rozlišení keyword/id a dokončení tokenu
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
                return 0;
            }
        break;

            //tvorba Integeru/doublu/exponentu
            case '2':
                if ((c >= '0' && c <= '9')){
                    token->attribute[i] = c;
                    i++;
                    //realloc při nedostatečné délce
                    if (i > (length - 1))
                    {
                        length += REALL_TOKEN_LEN;
                        token->attribute = realloc(token->attribute, length * sizeof(char));
                    }
                }
                //exponent
                else if(c == 'E' || c == 'e'){
                    token->attribute[i] = c;
                    i++;
                    if (i > (length - 1))
                    {
                        length += REALL_TOKEN_LEN;
                        token->attribute = realloc(token->attribute, length * sizeof(char));
                    }
                    c = getc(stdin);
                    if (c == '+' || c == '-')
                    {
                        token->attribute[i] = c;
                        i++;
                        if (i > (length - 1))
                        {
                            length += REALL_TOKEN_LEN;
                            token->attribute = realloc(token->attribute, length * sizeof(char));
                        }
                        c = getc(stdin);
                        if(c <= '0' || c >= '9'){
                            ungetc(c, stdin);
                            fprintf(stderr, "ERROR: picus on line %d!\n", token->line);
                            return 1;
                        }
                        else{
                            ungetc(c, stdin);
                        }
                    }
                    else if(c <= '0' || c >= '9'){
                        ungetc(c, stdin);
                        fprintf(stderr, "ERROR: Wrong format of an exponent on line %d!\n", token->line);
                        return 1;
                    }
                    else{
                        ungetc(c, stdin);
                    }
                    DoubleNumber = true;
                }
                else if(c == '.'){
                    token->attribute[i] = c;
                    i++;
                    if (i > (length - 1))
                    {
                        length += REALL_TOKEN_LEN;
                        token->attribute = realloc(token->attribute, length * sizeof(char));
                    }
                    c = getc(stdin);
                    if (c >= '0' && c <= '9')
                    {
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
                        fprintf(stderr, "ERROR: Wrong format of a number on line %d!\n", token->line);
                        return 1;
                    }
                    DoubleNumber = true;
                }
                //rozlišení double/int a dokončení tokenu
                else{
                    ungetc(c, stdin);
                    token->attribute[i] = '\0';
                    if (DoubleNumber){
                        strcpy(token->type, "number");
                        DoubleNumber = false;
                    }
                    else{
                        strcpy(token->type, "integer");
                    }
                
                    i = 0;
                    state = states[0];
                    return 0;
                }
            break;
        
        //stav pro rovná se nebo porovnání
        case '3':
            if (c == '='){
                token->attribute[i] = c;
                ++i;
                token->attribute[i] = '\0';
                strcpy(token->type, "equals");
                i = 0;
                
                state = states[0];
                return 0;
            }
            else{
                ungetc(c, stdin);
                token->attribute[i] = '\0';
                strcpy(token->type, "assign");
                i = 0;
                
                state = states[0];
                return 0;
            }
            
            
        break;
        
        //case pro tvorbu stringů
        case '4':
            token->attribute[i] = c;
            i++;
            if (i > (length - 1)){
                length += REALL_TOKEN_LEN;
                token->attribute = realloc(token->attribute, length * sizeof(char));
            }
            //escape sekvence
            if(c == '\\'){
                token->attribute[i] = c;
                i++;
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
                //escape sekvence která vytvoří znak
                else if(c >= '0' && c <='9'){
                    char escaped[4];
                    escaped[0] = c;
                    token->attribute[i] = c;
                    i++;
                    if (i > (length - 1))
                    {
                        length += REALL_TOKEN_LEN;
                        token->attribute = realloc(token->attribute, length * sizeof(char));
                    }
                    c = getc(stdin);
                    if (c >= '0' && c <='9'){
                        escaped[1] = c;
                        token->attribute[i] = c;
                        i++;
                        if (i > (length - 1))
                        {
                            length += REALL_TOKEN_LEN;
                            token->attribute = realloc(token->attribute, length * sizeof(char));
                        }
                        c = getc(stdin);
                        if (c >= '0' && c <='9'){
                            escaped[2] = c;
                            escaped[3] = '\0';
                            token->attribute[i] = c;
                            i++;
                            if (i > (length - 1))
                            {
                                length += REALL_TOKEN_LEN;
                                token->attribute = realloc(token->attribute, length * sizeof(char));
                            }
                            int aux = atoi(escaped);
                            if(aux <= 1 || aux >= 255){
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
            //dokončení stringu
            else if (c == '"')
            {
                if (strcmp(token->attribute, "\"ifj21\"")) {
                    token->attribute[i] = '\0';
                    for (int s = 0; token->attribute[s] != '\0'; s++){
                        token->attribute[s] = token->attribute[s+1];
                    }
                    
                    token->attribute[strlen(token->attribute)-1] = '\0';
                }
                strcpy(token->type, "string");
                i = 0;
                
                state = states[0];
                return 0;
            }
            //ošetření stavu když chybí jedny uvozovky
            else if(c == '\n' || c == EOF){
                fprintf(stderr, "ERROR: Unexpected End of file on line %d!", token->line);
                return 1;
            }
            
        break;

        //stav pro komentáře; scanner je ignoruje
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
            //blokový komentář
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
            //jinak pokud není komentář, je o mínus
            else if(!Comment){
                ungetc(c, stdin);
                token->attribute[i] = '\0';
                strcpy(token->type, "sub");
                i = 0;
                state = states[0];
                return 0;
            }
            
        break;

        //stavy pro porovnávání integerů
        case '6':
            if(c == '='){
                token->attribute[i] = c;
                ++i;
                token->attribute[i] = '\0';
                strcpy(token->type, "LEQT");
                i = 0;
                
                state = states[0];
                return 0;
            }
            else{
                ungetc(c, stdin);
                token->attribute[i] = '\0';
                strcpy(token->type, "LT");
                i = 0;
                
                state = states[0];
                return 0;
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
                return 0;
            }
            else{
                ungetc(c, stdin);
                token->attribute[i] = '\0';
                strcpy(token->type, "MT");
                i = 0;
                
                state = states[0];
                return 0;
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
                return 0;
            }
            else{
                fprintf(stderr, "ERROR: Missing \'=\' on %d\n", token->line);
                return 1;
            }

        break;

        //stav konkatenace
        case '9':
            if(c == '.'){
                token->attribute[i] = c;
                ++i;
                token->attribute[i] = '\0';
                strcpy(token->type, "concatenate");
                i = 0;
                
                state = states[0];
                return 0;
            }
            else{
                ungetc(c, stdin);
                token->attribute[i] = '\0';
                strcpy(token->type, "dot");
                i = 0;
                
                state = states[0];
                return 0;
            }
        break;

        //stav celočíselného dělení
        case 'd':
            if(c == '/'){
                token->attribute[i] = c;
                ++i;
                token->attribute[i] = '\0';
                strcpy(token->type, "IntegerDiv");
                i = 0;
                
                state = states[0];
                return 0;
            }
            else{
                ungetc(c, stdin);
                token->attribute[i] = '\0';
                strcpy(token->type, "div");
                i = 0;
                
                state = states[0];
                return 0;
            }
        break;

        default:
            
            break;
        }
        
    }
    return 0;
}

void printToken(tToken *token){
    printf("token attribute: %s\n", token->attribute);
    printf("token type: %s\n", token->type);
    printf("token line: %d\n", token->line);
    printf("\n");   
}

void initToken(tToken *token){
    token->attribute = NULL;
    token->type = NULL;
}

void deleteToken(tToken *token){
    free(token->attribute);
    free(token->type);
    free(token);
    token = NULL;
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
void STokenFinish(tToken *token, int *i, char *string, char c){
    token->attribute[*i] = c;
    ++(*i);
    token->attribute[*i] = '\0';
    strcpy(token->type, string);
    *i = 0;
}

tToken *token_init()
{
    tToken *token = malloc(sizeof(struct Token));
    if(token == NULL){
        fprintf(stderr, "ERROR: Failed malloc");
        return NULL;
    } 
    int length = TOKEN_LENGTH;
    token->attribute = malloc(length * sizeof(char));
    if(token->attribute == NULL){
        fprintf(stderr, "ERROR: Failed malloc");
        return NULL;
    }
    token->type = malloc(15 * sizeof(char));
    if(token->type == NULL){
        fprintf(stderr, "ERROR: Failed malloc");
        return NULL;
    }
    token->line = 1;
    return token;
}
