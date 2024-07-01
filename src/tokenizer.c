#include "tokenizer.h"
#include "arraylist.h"
#include "hashmap.h"
#include "stringtype.h"
#include "file.h"


#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


typedef struct {
   ArrayList tokens;
   HashMap keywords;
   int line;
   File* file;
   char currentChar;
   char prevChar;
   String currentString;
} Tokenizer;


static void tokenizer_create(Tokenizer *tokenizer, File* file){
    array_list_create(tokenizer->tokens, Token);
    tokenizer->line = 1;
    tokenizer->file = file;
    tokenizer->currentChar = '\0';
    tokenizer->prevChar = '\0';
    tokenizer->currentString = string_create();
    tokenizer->keywords = hash_map_create(42, NULL); //even though there are only 32 keywords 42 seems to be optimal capacity 
    hash_map_add_kv(&tokenizer->keywords, "False", TokenType, TOK_FALSE);
    hash_map_add_kv(&tokenizer->keywords, "True", TokenType, TOK_TRUE);
    hash_map_add_kv(&tokenizer->keywords, "None", TokenType, TOK_NONE);
    hash_map_add_kv(&tokenizer->keywords, "and", TokenType, TOK_AND);
    hash_map_add_kv(&tokenizer->keywords, "as", TokenType, TOK_AS);
    hash_map_add_kv(&tokenizer->keywords, "assert", TokenType, TOK_ASSERT);
    hash_map_add_kv(&tokenizer->keywords, "break", TokenType, TOK_BREAK);
    hash_map_add_kv(&tokenizer->keywords, "class", TokenType, TOK_CLASS);
    hash_map_add_kv(&tokenizer->keywords, "continue", TokenType, TOK_CONTINUE);
    hash_map_add_kv(&tokenizer->keywords, "def", TokenType, TOK_DEF);
    hash_map_add_kv(&tokenizer->keywords, "del", TokenType, TOK_DEL);
    hash_map_add_kv(&tokenizer->keywords, "elif", TokenType, TOK_ELIF);
    hash_map_add_kv(&tokenizer->keywords, "else", TokenType, TOK_ELSE);
    hash_map_add_kv(&tokenizer->keywords, "except", TokenType, TOK_EXCEPT);
    hash_map_add_kv(&tokenizer->keywords, "finally", TokenType, TOK_FINALLY);
    hash_map_add_kv(&tokenizer->keywords, "for", TokenType, TOK_FOR);
    hash_map_add_kv(&tokenizer->keywords, "from", TokenType, TOK_FROM);
    hash_map_add_kv(&tokenizer->keywords, "global", TokenType, TOK_GLOBAL);
    hash_map_add_kv(&tokenizer->keywords, "import", TokenType, TOK_IMPORT);
    hash_map_add_kv(&tokenizer->keywords, "if", TokenType, TOK_IF);
    hash_map_add_kv(&tokenizer->keywords, "is", TokenType, TOK_IS);
    hash_map_add_kv(&tokenizer->keywords, "lambda", TokenType, TOK_LAMBDA);
    hash_map_add_kv(&tokenizer->keywords, "in", TokenType, TOK_IN);
    hash_map_add_kv(&tokenizer->keywords, "nonlocal", TokenType, TOK_NONLOCAL);
    hash_map_add_kv(&tokenizer->keywords, "not", TokenType, TOK_NOT);
    hash_map_add_kv(&tokenizer->keywords, "pass", TokenType, TOK_PASS);
    hash_map_add_kv(&tokenizer->keywords, "raise", TokenType, TOK_RAISE);
    hash_map_add_kv(&tokenizer->keywords, "return", TokenType, TOK_RETURN);
    hash_map_add_kv(&tokenizer->keywords, "or", TokenType, TOK_OR);
    hash_map_add_kv(&tokenizer->keywords, "yield", TokenType, TOK_YIELD);
    hash_map_add_kv(&tokenizer->keywords, "with", TokenType, TOK_WITH);
    hash_map_add_kv(&tokenizer->keywords, "while", TokenType, TOK_WHILE);
    hash_map_add_kv(&tokenizer->keywords, "try", TokenType, TOK_TRY);
}


static ArrayList delete_tokenizer(Tokenizer *tokenizer){
    hash_map_delete(&tokenizer->keywords);
    string_delete(&tokenizer->currentString);
    return tokenizer->tokens;
}

static char next_char(Tokenizer *tokenizer){
    tokenizer->prevChar = tokenizer->currentChar;
    tokenizer->currentChar = file_next_char(tokenizer->file);
    return tokenizer->currentChar;
}

static char peek_char(Tokenizer *tokenizer){
    return file_peek(tokenizer->file);
}

static char prev_char(Tokenizer *tokenizer){
    return tokenizer->prevChar;
}


TokenType get_prev_token(Tokenizer *tokenizer){
    if(tokenizer->tokens.size != 0) {
        Token t = array_list_get(tokenizer->tokens, Token, tokenizer->tokens.size - 1);
        return t.type;
    }
    return TOK_MAX;
}


void token_delete(Token* t){
    t->type = TOK_UNKOWN;
    string_delete(&t->literal);
}

static void add_token(Tokenizer *tokenizer, TokenType type){
    Token token;
    token.type = type;
    token.line = tokenizer->line;
    switch (type) {
        case TOK_IDENTIFIER:
        case TOK_FLOAT:
        case TOK_STRING:
        case TOK_INTEGER: 
            token.literal = string_from_str(tokenizer->currentString.str); 
            string_clear(&tokenizer->currentString);
            break;
        default: 
            token.literal = DEFAULT_STR; 
    }
    array_list_append(tokenizer->tokens, Token, token);
}



/** 
 * Determing if the string is a keyword, float, int, or identifier 
 */
static void check_misc_string(Tokenizer *tokenizer){
    //check for keyword
    TokenType* keyword = hash_map_get_value(&tokenizer->keywords, tokenizer->currentString.str);
    if(keyword != NULL){
        add_token(tokenizer,*keyword);
    }
    //checking if we have an integer or a float 
    else if(isdigit(tokenizer->currentString.str[0])){
        //if it contains a period it is likely a float 
        if(strchr(tokenizer->currentString.str, '.') != NULL){
            double d = strtod(tokenizer->currentString.str, NULL);
            //verify that it is actually a float 
            if(fabs(d - 0.0) < 0.00001 && strcmp(tokenizer->currentString.str, "0.0") != 0){
                fprintf(stderr, "Invalid float on line %d\n", tokenizer->line);
                exit(2);
            } else{
                add_token(tokenizer, TOK_FLOAT);
            }
        } else{
            //if not a string check if its an integer 
            int integer = atoi(tokenizer->currentString.str);
            if((integer == 0 && tokenizer->currentString.size == 1 && tokenizer->currentString.str[0] == '0') || integer != 0){
                add_token(tokenizer, TOK_INTEGER);
            } else {
                fprintf(stderr, "Invalid int on line %d\n", tokenizer->line);
                exit(2);
            }
        } 
    }    
    else{
        add_token(tokenizer, TOK_IDENTIFIER);
    } 
    string_clear(&tokenizer->currentString);
}


static void add_token_check_string(Tokenizer *tokenizer, TokenType type){
    if(tokenizer->currentString.size != 0){
        check_misc_string(tokenizer);
    }
    add_token(tokenizer, type);
}



static void create_string_token(Tokenizer *tokenizer){
    if(tokenizer->currentString.size != 0){
        check_misc_string(tokenizer);
    }
    //consume quote char 
    next_char(tokenizer);
    while(tokenizer->currentChar != '\"'){
        string_push(&tokenizer->currentString, tokenizer->currentChar);
        next_char(tokenizer);

        if(tokenizer->currentChar == EOF){
            fprintf(stderr, "Unterminated string\n");
            exit(1);
        }
    }

    add_token(tokenizer, TOK_STRING);
    string_clear(&tokenizer->currentString);
}

//converts four spaces to a tab 
static void spaces_to_tab(Tokenizer *tokenizer){
    int space_counter = 0;
    while(tokenizer->currentChar == ' '){
        space_counter++;
        if(space_counter == 4){
            add_token(tokenizer, TOK_TAB);
            space_counter = 0;
        } 
        if(peek_char(tokenizer) != ' ') break;
        next_char(tokenizer);
    }

    if(space_counter != 0){
        fprintf(stderr, "Invalid identation. Indents must be 4 spaces or a tab on line %d\n", tokenizer->line);
        exit(1);
    }
}




//checks to see if the next char makes it an assignment operator 
//+= or if its is an = it becomes ==  
#define check_assign(tokenizer, assign_tok, normal_tok) \
    if(peek_char(&tokenizer) == '='){ \
        add_token_check_string(&tokenizer, assign_tok); \
        next_char(&tokenizer); \
    } else { \
        add_token_check_string(&tokenizer, normal_tok); \
    } \


ArrayList tokenize_file(File* file){

    Tokenizer tokenizer = {0};
    tokenizer_create(&tokenizer, file);
 
    while(true) {
        next_char(&tokenizer);

        //TODO: ADD MULTILINE COMMENTS
        switch (tokenizer.currentChar) {
            case ' ':
                if(prev_char(&tokenizer) == '\n'){
                    spaces_to_tab(&tokenizer);
                } 
                if(tokenizer.currentString.size != 0) {
                        check_misc_string(&tokenizer);
                }
                break;
            case '=':
                check_assign(tokenizer,TOK_EQUAL, TOK_ASSIGN); 
                break;  
            case '\n':
                if(tokenizer.currentString.size != 0) check_misc_string(&tokenizer);
                tokenizer.line++;
                if(prev_char(&tokenizer) == '\n') break; //ignore multiple new lines
                add_token(&tokenizer, TOK_NEW_LINE);
                break;
            case '\t':
                if(get_prev_token(&tokenizer) != TOK_NEW_LINE || get_prev_token(&tokenizer) != TOK_TAB){
                    fprintf(stderr, "Invalid block\n");
                    exit(1);
                }
                add_token_check_string(&tokenizer,TOK_TAB);
                break;
            case '(':
                add_token_check_string(&tokenizer,TOK_LEFT_PAREN);
                break; 
            case ')':
                add_token_check_string(&tokenizer,TOK_RIGHT_PAREN);
                break; 
            case ':':
                add_token_check_string(&tokenizer,TOK_COLON);
                break;
            case ',':
                add_token_check_string(&tokenizer, TOK_COMMA);
                break;
            case '+':
                check_assign(tokenizer,TOK_ADD_ASSIGN, TOK_ADD); 
                break;
            case '-':
                check_assign(tokenizer,TOK_SUB_ASSIGN, TOK_SUB);
                break;
            case '*': {
                          switch (peek_char(&tokenizer)) {
                              case '*':
                                  next_char(&tokenizer);
                                  check_assign(tokenizer,TOK_EXP_ASSIGN, TOK_EXP);
                                  break;                                  break;
                              case '=':
                                  add_token_check_string(&tokenizer,TOK_MUL_ASSIGN);
                                  next_char(&tokenizer);
                                  break;
                              default:
                                  add_token_check_string(&tokenizer,TOK_MUL);
                          }
                          break;
                      }
            case '!': {
                          if(next_char(&tokenizer) == '='){
                              add_token_check_string(&tokenizer,TOK_NOT_EQUAL);
                          } else {
                              fprintf(stderr, "Invalid token\n");
                          }
                          break;
                      }
            case '/': {
                          switch (peek_char(&tokenizer)) {
                              case '/':
                                  next_char(&tokenizer);
                                  check_assign(tokenizer,TOK_FLOOR_DIV_ASSIGN, TOK_FLOOR_DIV);
                                  break;
                              case '=':
                                  add_token_check_string(&tokenizer,TOK_DIV_ASSIGN);
                                  next_char(&tokenizer);
                                  break;
                              default:
                                  add_token_check_string(&tokenizer,TOK_DIV);
                          }
                          break;
                      }
            case '%':
                      check_assign(tokenizer,TOK_MOD_ASSIGN, TOK_MOD);
                      break; 
            case '<': {
                          switch (peek_char(&tokenizer)) {
                              case '=':
                                  add_token_check_string(&tokenizer,TOK_LESS_EQUAL);
                                  next_char(&tokenizer);
                                  break;
                              case '<':
                                  next_char(&tokenizer);
                                  check_assign(tokenizer,TOK_LEFT_SHIFT_ASSIGN, TOK_LEFT_SHIFT);
                                  break;
                              default:
                                  add_token_check_string(&tokenizer,TOK_LESS_THAN);
                          }
                          break;
                      }
            case '>': {
                          switch (peek_char(&tokenizer)) {
                              case '=':
                                  add_token_check_string(&tokenizer,TOK_GREATER_EQUAL);
                                  next_char(&tokenizer);
                                  break;
                              case '>':
                                  next_char(&tokenizer);
                                  check_assign(tokenizer,TOK_RIGHT_SHIFT_ASSIGN, TOK_RIGHT_SHIFT);
                                  break;
                              default:
                                  add_token_check_string(&tokenizer,TOK_GREATER_THAN); 
                          }
                          break;
                      }
            case '\"': 
                      create_string_token(&tokenizer);
                      break;
            case '\'':
                      add_token_check_string(&tokenizer,TOK_SQUOTE);
                      break;
            case '[': 
                      add_token_check_string(&tokenizer,TOK_LEFT_BRACKET);
                      break;
            case ']': 
                      add_token_check_string(&tokenizer,TOK_RIGHT_BRACKET);
                      break;
            case '{':   
                      add_token_check_string(&tokenizer,TOK_LEFT_BRACE);
                      break;
            case '}':
                      add_token_check_string(&tokenizer,TOK_RIGHT_BRACE);
                      break;
            case '#':
                      add_token_check_string(&tokenizer,TOK_NEW_LINE);
                      file_end_line(tokenizer.file);
                      break;
            case '.':
                      add_token_check_string(&tokenizer,TOK_DOT);
                      break;
            case '&':
                      check_assign(tokenizer,TOK_BIT_AND_ASSIGN, TOK_BIT_AND);
                      break;
            case '|':
                      check_assign(tokenizer,TOK_BIT_OR_ASSIGN, TOK_BIT_OR);
                      break;
            case '^':
                      check_assign(tokenizer,TOK_BIT_XOR_ASSIGN, TOK_BIT_XOR);
                      break;
            case '~':
                      add_token_check_string(&tokenizer,TOK_BIT_NOT);
                      break;

                      //new lines are typically used to end statements but semicolons can also be used 
                      //so we just convert semicolons to new lines 
            case ';':
                      add_token_check_string(&tokenizer,TOK_NEW_LINE);
                      break;
            case '@':
                      printf("Decorators are not supported\n");
                      break;
            default:
                if(isalnum(tokenizer.currentChar) || tokenizer.currentChar == '_'){
                    string_push(&tokenizer.currentString, tokenizer.currentChar);
                }
                else if(tokenizer.currentChar == EOF){
                    add_token(&tokenizer, TOK_EOF);
                    goto END;
                }
                else{
                    add_token(&tokenizer, TOK_UNKOWN);
                }
        }

    }
END:
     return delete_tokenizer(&tokenizer);
}






