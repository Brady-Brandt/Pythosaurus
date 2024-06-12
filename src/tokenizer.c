#include "tokenizer.h"
#include "arraylist.h"
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
}



static char next_char(){
    tokenizer.prevChar = tokenizer.currentChar;
    tokenizer.currentChar = file_next_char(&tokenizer.file);
    return tokenizer.currentChar;
}


static char get_char(){
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
 * TODO: USE HASHMAP FOR BETTER PERFORMANCE
 */
static void check_misc_string(String* string){
    //checking if we have an integer or a float 
    if(isdigit(string->str[0])){
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

    else if(strcmp(string->str, "False") == 0) {
        add_token(TOK_FALSE);
    }
    else if(strcmp(string->str, "None") == 0) {
        add_token(TOK_NONE);
    }
    else if(strcmp(string->str, "True") == 0) {
        add_token(TOK_TRUE);
    }
    else if(strcmp(string->str, "and") == 0) {
        add_token(TOK_AND);
    }
    else if(strcmp(string->str, "as") == 0) {
        add_token(TOK_AS);
    }
    else if(strcmp(string->str, "assert") == 0) {
        add_token(TOK_ASSERT);
    } 
    else if(strcmp(string->str, "break") == 0) {
        add_token(TOK_BREAK);
    }
    else if(strcmp(string->str, "class") == 0) {
        add_token(TOK_CLASS);
    }
    else if(strcmp(string->str, "continue") == 0) {
        add_token(TOK_CONTINUE); 
    }
    else if(strcmp(string->str, "def") == 0) {
        add_token(TOK_DEF);
    }
    else if(strcmp(string->str, "del") == 0) {
        add_token(TOK_DEL);
    }
    else if(strcmp(string->str, "elif") == 0) {
        add_token(TOK_ELIF);
    }
    else if(strcmp(string->str, "else") == 0) {
        add_token(TOK_ELSE);
    }
    else if(strcmp(string->str, "except") == 0) {
        add_token(TOK_EXCEPT);
    }
    else if(strcmp(string->str, "finally") == 0) {
        add_token(TOK_FINALLY);
    }
    else if(strcmp(string->str, "for") == 0) {
        add_token(TOK_FOR);
    }
    else if(strcmp(string->str, "from") == 0) {
        add_token(TOK_FROM);
    }
    else if(strcmp(string->str, "global") == 0) {
        add_token(TOK_GLOBAL);
    }
    else if(strcmp(string->str, "if") == 0) {
        add_token(TOK_IF);
    }
    else if(strcmp(string->str, "import") == 0) {
        add_token(TOK_IMPORT);
    }
    else if(strcmp(string->str, "in") == 0) {
        add_token(TOK_IN);
    }
    else if(strcmp(string->str, "is") == 0) {
        add_token(TOK_IS);
    }
    else if(strcmp(string->str, "lambda") == 0) {
        add_token(TOK_LAMBDA);
    }
    else if(strcmp(string->str, "nonlocal") == 0) {
        add_token(TOK_NONLOCAL);
    }
    else if(strcmp(string->str, "not") == 0) {
        add_token(TOK_NOT);
    }
    else if(strcmp(string->str, "or") == 0) {
        add_token(TOK_OR);
    }
    else if(strcmp(string->str, "pass") == 0) {
        add_token(TOK_PASS);
    }
    else if(strcmp(string->str, "raise") == 0) {
        add_token(TOK_RAISE);
    }
    else if(strcmp(string->str, "return") == 0) {
        add_token(TOK_RETURN);
    }
    else if(strcmp(string->str, "try") == 0) {
        add_token(TOK_TRY);
    }
    else if(strcmp(string->str, "while") == 0) {
        add_token(TOK_WHILE);
    }
    else if(strcmp(string->str, "with") == 0) {
        add_token(TOK_WITH);
    }
    else if(strcmp(string->str, "yield") == 0) {
        add_token(TOK_YIELD);
    } else{
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
                if(get_prev_token() == TOK_NEW_LINE) break;
                tokenizer.line++;
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
                if(isalnum(tokenizer.currentChar)){
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
    file_close(&tokenizer.file);
    return tokenizer.tokens;
}






