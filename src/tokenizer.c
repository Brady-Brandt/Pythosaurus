#include "tokenizer.h"
#include "arraylist.h"
#include "hashmap.h"
#include "stringtype.h"
#include "stack.h"
#include "file.h"


#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


Tokenizer tokenizer = {0};


void tokenizer_create(const char* file_name){
    File f = file_open(file_name); 
    array_list_create(tokenizer.tokens, Token);
    tokenizer.line = 1;
    tokenizer.file = f;
    tokenizer.currentChar = '\0';
    tokenizer.prevChar = '\0';
    tokenizer.keywords = hash_map_create(42); //even though there are only 32 keywords 42 seems to be optimal capacity 
    hash_map_add_kv(&tokenizer.keywords, "False", TokenType, TOK_FALSE);
    hash_map_add_kv(&tokenizer.keywords, "True", TokenType, TOK_TRUE);
    hash_map_add_kv(&tokenizer.keywords, "None", TokenType, TOK_NONE);
    hash_map_add_kv(&tokenizer.keywords, "and", TokenType, TOK_AND);
    hash_map_add_kv(&tokenizer.keywords, "as", TokenType, TOK_AS);
    hash_map_add_kv(&tokenizer.keywords, "assert", TokenType, TOK_ASSERT);
    hash_map_add_kv(&tokenizer.keywords, "break", TokenType, TOK_BREAK);
    hash_map_add_kv(&tokenizer.keywords, "class", TokenType, TOK_CLASS);
    hash_map_add_kv(&tokenizer.keywords, "continue", TokenType, TOK_CONTINUE);
    hash_map_add_kv(&tokenizer.keywords, "def", TokenType, TOK_DEF);
    hash_map_add_kv(&tokenizer.keywords, "del", TokenType, TOK_DEL);
    hash_map_add_kv(&tokenizer.keywords, "elif", TokenType, TOK_ELIF);
    hash_map_add_kv(&tokenizer.keywords, "else", TokenType, TOK_ELSE);
    hash_map_add_kv(&tokenizer.keywords, "except", TokenType, TOK_EXCEPT);
    hash_map_add_kv(&tokenizer.keywords, "finally", TokenType, TOK_FINALLY);
    hash_map_add_kv(&tokenizer.keywords, "for", TokenType, TOK_FOR);
    hash_map_add_kv(&tokenizer.keywords, "from", TokenType, TOK_FROM);
    hash_map_add_kv(&tokenizer.keywords, "global", TokenType, TOK_GLOBAL);
    hash_map_add_kv(&tokenizer.keywords, "import", TokenType, TOK_IMPORT);
    hash_map_add_kv(&tokenizer.keywords, "if", TokenType, TOK_IF);
    hash_map_add_kv(&tokenizer.keywords, "is", TokenType, TOK_IS);
    hash_map_add_kv(&tokenizer.keywords, "lambda", TokenType, TOK_LAMBDA);
    hash_map_add_kv(&tokenizer.keywords, "in", TokenType, TOK_IN);
    hash_map_add_kv(&tokenizer.keywords, "nonlocal", TokenType, TOK_NONLOCAL);
    hash_map_add_kv(&tokenizer.keywords, "not", TokenType, TOK_NOT);
    hash_map_add_kv(&tokenizer.keywords, "pass", TokenType, TOK_PASS);
    hash_map_add_kv(&tokenizer.keywords, "raise", TokenType, TOK_RAISE);
    hash_map_add_kv(&tokenizer.keywords, "return", TokenType, TOK_RETURN);
    hash_map_add_kv(&tokenizer.keywords, "or", TokenType, TOK_OR);
    hash_map_add_kv(&tokenizer.keywords, "yield", TokenType, TOK_YIELD);
    hash_map_add_kv(&tokenizer.keywords, "with", TokenType, TOK_WITH);
    hash_map_add_kv(&tokenizer.keywords, "while", TokenType, TOK_WHILE);
    hash_map_add_kv(&tokenizer.keywords, "try", TokenType, TOK_TRY);
    
}


static ArrayList delete_tokenizer(){
    hash_map_delete(&tokenizer.keywords);
    file_close(&tokenizer.file);
    return tokenizer.tokens;
}

static char next_char(){
    tokenizer.prevChar = tokenizer.currentChar;
    tokenizer.currentChar = file_next_char(&tokenizer.file);
    return tokenizer.currentChar;
}

static char peek_char(){
    return file_peek(&tokenizer.file);
}

static char prev_char(){
    return tokenizer.prevChar;
}


TokenType get_prev_token(){
    if(tokenizer.tokens.size != 0) {
        Token t = array_list_get(tokenizer.tokens, Token, tokenizer.tokens.size - 1);
        return t.type;
    }
    return TOK_MAX;
}


void token_delete(Token* t){
    t->type = TOK_UNKOWN;
    string_delete(&t->literal);
}

static void add_token(TokenType type){
    Token token;
    token.type = type;
    token.literal = DEFAULT_STR;
    array_list_append(tokenizer.tokens, Token, token);
}


static void add_token_with_string(TokenType type, String* string){
    Token token;
    token.type = type;
    token.literal = string_from_str(string->str);
    string_clear(string);
    array_list_append(tokenizer.tokens, Token, token); 
}



/** 
 * Determing if the string is a keyword, float, int, or identifier 
 */
static void check_misc_string(String* string){
    //check for keyword
    TokenType* keyword = hash_map_get_value(&tokenizer.keywords, string->str);
    if(keyword != NULL){
        add_token(*keyword);
    }
    //checking if we have an integer or a float 
    else if(isdigit(string->str[0])){
        //if it contains a period it is likely a float 
        if(strchr(string->str, '.') != NULL){
            double d = strtod(string->str, NULL);
            //verify that it is actually a float 
            if(fabs(d - 0.0) < 0.00001 && strcmp(string->str, "0.0") != 0){
                fprintf(stderr, "Invalid float on line %d\n", tokenizer.line);
                exit(2);
            } else{
                add_token_with_string(TOK_FLOAT, string);
            }
        } else{
            //if not a string check if its an integer 
            int integer = atoi(string->str);
            if((integer == 0 && string->size == 1 && string->str[0] == '0') || integer != 0){
                add_token_with_string(TOK_INTEGER, string);
            } else {
                fprintf(stderr, "Invalid int on line %d\n", tokenizer.line);
                exit(2);
            }
        } 
    }    
    else{
        add_token_with_string(TOK_IDENTIFIER, string);
    } 
    string_clear(string);
}


static void add_token_check_misc(TokenType type, String* misc){
    if(misc->size != 0){
        check_misc_string(misc);
    }
    add_token(type);
}


void validate_enclosures(Stack* s, char closing){
    if(stack_is_empty(s)){
        fprintf(stderr, "Unclosed %c on line %d\n", closing, tokenizer.line);
        exit(2);
    }

    char opening = stack_pop(s);
    if(opening == '(' && closing != ')'){
        fprintf(stderr, "Unclosed ( on line %d\n", tokenizer.line);
        exit(2);
    }
    else if(opening == '[' && closing != ']'){
        fprintf(stderr, "Unclosed [ on line %d\n", tokenizer.line);
        exit(2);
    }
    else if(opening == '{' && closing != '}'){
        fprintf(stderr, "Unclosed { on line %d\n", tokenizer.line);
        exit(2);
    }
    else if(opening == '\"' && closing != '\"'){
        fprintf(stderr, "Unclosed \" on line %d\n", tokenizer.line);
        exit(2);
    }
}


static void create_string_token(String* str){
    if(str->size != 0){
        check_misc_string(str);
    }
    //consume quote char 
    next_char();
    while(tokenizer.currentChar != '\"'){
        string_push(str, tokenizer.currentChar);
        next_char();

        if(tokenizer.currentChar == EOF){
            fprintf(stderr, "Unterminated string\n");
            exit(1);
        }
    }

    add_token_with_string(TOK_STRING, str);
    string_clear(str);
}

//converts four spaces to a tab 
static void spaces_to_tab(){
    int space_counter = 0;
    while(tokenizer.currentChar == ' '){
        space_counter++;
        if(space_counter == 4){
            add_token(TOK_TAB);
            space_counter = 0;
        } 
        if(peek_char() != ' ') break;
        next_char();
    }

    if(space_counter != 0){
        printf("%d\n", space_counter);
        fprintf(stderr, "Invalid identation. Idents must be 4 spaces or a tab on line %d\n", tokenizer.line);
        exit(1);
    }
}




//checks to see if the next char makes it an assignment operator 
//+= or if its is an = it becomes ==  
#define check_assign(assign_tok, normal_tok, literal) \
    if(peek_char() == '='){ \
        add_token_check_misc(assign_tok, literal); \
        next_char(); \
    } else { \
        add_token_check_misc(normal_tok, literal); \
    } \


ArrayList tokenize_file(){

    //holds a string, keyword, float, or int 
    String curr_literal = string_create();
    Stack enclosure_stack;
    stack_create(&enclosure_stack);


    while(true) {
        next_char();

        //TODO: ADD MULTILINE COMMENTS
        switch (tokenizer.currentChar) {
            case ' ':
                if(prev_char() == '\n'){
                    spaces_to_tab();
                } 
                if(curr_literal.size != 0) {
                        check_misc_string(&curr_literal);
                        string_clear(&curr_literal);
                }
                break;
            case '=':
                check_assign(TOK_EQUAL, TOK_ASSIGN, &curr_literal); 
                break;  
            case '\n':
                tokenizer.line++;
                if(prev_char() == '\n') break; //ignore multiple new lines
                add_token_check_misc(TOK_NEW_LINE, &curr_literal);
                break;
            case '\t':
                if(get_prev_token() != TOK_NEW_LINE || get_prev_token() != TOK_TAB){
                    fprintf(stderr, "Invalid block\n");
                    exit(1);
                }
                add_token_check_misc(TOK_TAB, &curr_literal);
                break;
            case '(':
                stack_push(&enclosure_stack, tokenizer.currentChar);
                add_token_check_misc(TOK_LEFT_PAREN, &curr_literal);
                break; 
            case ')':
                validate_enclosures(&enclosure_stack, tokenizer.currentChar);
                add_token_check_misc(TOK_RIGHT_PAREN, &curr_literal);
                break; 
            case ':':
                add_token_check_misc(TOK_COLON, &curr_literal);
                break;
            case ',':
                add_token_check_misc(TOK_COMMA, &curr_literal);
                break;
            case '+':
                check_assign(TOK_ADD_ASSIGN, TOK_ADD, &curr_literal); 
                break;
            case '-':
                check_assign(TOK_SUB_ASSIGN, TOK_SUB, &curr_literal);
                break;
            case '*': {
                          switch (peek_char()) {
                              case '*':
                                  next_char();
                                  check_assign(TOK_EXP_ASSIGN, TOK_EXP, &curr_literal);
                                  break;                                  break;
                              case '=':
                                  add_token_check_misc(TOK_MUL_ASSIGN, &curr_literal);
                                  next_char();
                                  break;
                              default:
                                  add_token_check_misc(TOK_MUL, &curr_literal);
                          }
                          break;
                      }
            case '!': {
                          if(next_char() == '='){
                              add_token_check_misc(TOK_NOT_EQUAL, &curr_literal);
                          } else {
                              fprintf(stderr, "Invalid token\n");
                          }
                          break;
                      }
            case '/': {
                          switch (peek_char()) {
                              case '/':
                                  next_char();
                                  check_assign(TOK_FLOOR_DIV_ASSIGN, TOK_FLOOR_DIV, &curr_literal);
                                  break;
                              case '=':
                                  add_token_check_misc(TOK_DIV_ASSIGN, &curr_literal);
                                  next_char();
                                  break;
                              default:
                                  add_token_check_misc(TOK_DIV, &curr_literal);
                          }
                          break;
                      }
            case '%':
                      check_assign(TOK_MOD_ASSIGN, TOK_MOD, &curr_literal);
                      break; 
            case '<': {
                          switch (peek_char()) {
                              case '=':
                                  add_token_check_misc(TOK_LESS_EQUAL, &curr_literal);
                                  next_char();
                                  break;
                              case '<':
                                  next_char();
                                  check_assign(TOK_LEFT_SHIFT_ASSIGN, TOK_LEFT_SHIFT, &curr_literal);
                                  break;
                              default:
                                  add_token_check_misc(TOK_LESS_THAN, &curr_literal);
                          }
                          break;
                      }
            case '>': {
                          switch (peek_char()) {
                              case '=':
                                  add_token_check_misc(TOK_GREATER_EQUAL, &curr_literal);
                                  next_char();
                                  break;
                              case '>':
                                  next_char();
                                  check_assign(TOK_RIGHT_SHIFT_ASSIGN, TOK_RIGHT_SHIFT, &curr_literal);
                                  break;
                              default:
                                  add_token_check_misc(TOK_GREATER_THAN, &curr_literal); 
                          }
                          break;
                      }
            case '\"': 
                      create_string_token(&curr_literal);
                      break;
            case '\'':
                      add_token_check_misc(TOK_SQUOTE, &curr_literal);
                      break;
            case '[': 
                      stack_push(&enclosure_stack, tokenizer.currentChar);
                      add_token_check_misc(TOK_LEFT_BRACKET, &curr_literal);
                      break;
            case ']': 
                      validate_enclosures(&enclosure_stack, tokenizer.currentChar);
                      add_token_check_misc(TOK_RIGHT_BRACKET, &curr_literal);
                      break;
            case '{':   
                      stack_push(&enclosure_stack, tokenizer.currentChar);
                      add_token_check_misc(TOK_LEFT_BRACE, &curr_literal);
                      break;
            case '}':
                      validate_enclosures(&enclosure_stack, tokenizer.currentChar);
                      add_token_check_misc(TOK_RIGHT_BRACE, &curr_literal);
                      break;
            case '#':
                      add_token_check_misc(TOK_NEW_LINE, &curr_literal);
                      file_end_line(&tokenizer.file);
                      break;
            case '.':
                      add_token_check_misc(TOK_DOT, &curr_literal);
                      break;
            case '&':
                      check_assign(TOK_BIT_AND_ASSIGN, TOK_BIT_AND, &curr_literal);
                      break;
            case '|':
                      check_assign(TOK_BIT_OR_ASSIGN, TOK_BIT_OR, &curr_literal);
                      break;
            case '^':
                      check_assign(TOK_BIT_XOR_ASSIGN, TOK_BIT_XOR, &curr_literal);
                      break;
            case '~':
                      add_token_check_misc(TOK_BIT_NOT, &curr_literal);
                      break;

                      //new lines are typically used to end statements but semicolons can also be used 
                      //so we just convert semicolons to new lines 
            case ';':
                      add_token_check_misc(TOK_NEW_LINE, &curr_literal);
                      break;
            case '@':
                      printf("Decorators are not supported\n");
                      break;
            default:
                if(isalnum(tokenizer.currentChar) || tokenizer.currentChar == '_'){
                    string_push(&curr_literal, tokenizer.currentChar);
                }
                else if(tokenizer.currentChar == EOF){
                    add_token(TOK_EOF);
                    goto END;
                }
                else{
                    fprintf(stderr, "Invalid token %c on line %d\n", tokenizer.currentChar, tokenizer.line);
                    exit(2);
                }
        }

    }
END: 
    string_delete(&curr_literal);
    ArrayList tokens = delete_tokenizer();
    return tokens;
}






