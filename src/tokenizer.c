#include "tokenizer.h"
#include "arraylist.h"
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
    }


static ArrayList delete_tokenizer(Tokenizer *tokenizer){
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
    //pretty much just as fast as using a hashmap 
    if(strcmp("False", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_FALSE);
    else if(strcmp("True", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_TRUE);
    else if(strcmp("None", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_NONE);
    else if(strcmp("and", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_AND);
    else if(strcmp("as", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_AS);
    else if(strcmp("assert", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_ASSERT);
    else if(strcmp("break", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_BREAK);
    else if(strcmp("class", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_CLASS);
    else if(strcmp("continue", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_CONTINUE);
    else if(strcmp("def", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_DEF);
    else if(strcmp("del", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_DEL);
    else if(strcmp("elif", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_ELIF);
    else if(strcmp("else", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_ELSE);
    else if(strcmp("except", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_EXCEPT);
    else if(strcmp("finally", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_FINALLY);
    else if(strcmp("for", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_FOR);
    else if(strcmp("from", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_FROM);
    else if(strcmp("global", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_GLOBAL);
    else if(strcmp("import", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_IMPORT);
    else if(strcmp("if", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_IF);
    else if(strcmp("is", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_IS);
    else if(strcmp("lambda", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_LAMBDA);
    else if(strcmp("in", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_IN);
    else if(strcmp("nonlocal", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_NONLOCAL);
    else if(strcmp("not", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_NOT);
    else if(strcmp("pass", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_PASS);
    else if(strcmp("raise", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_RAISE);
    else if(strcmp("return", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_RETURN);
    else if(strcmp("or", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_OR);
    else if(strcmp("yield", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_YIELD);
    else if(strcmp("with", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_WITH);
    else if(strcmp("while", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_WHILE);
    else if(strcmp("try", tokenizer->currentString.str) == 0) add_token(tokenizer, TOK_TRY);
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
                              add_token(&tokenizer, TOK_UNKOWN);
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






