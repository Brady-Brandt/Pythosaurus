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

#define DEBUG

#ifdef DEBUG
    const char* token_names[] = {
        //keywords
        "TOK_FALSE",
        "TOK_NONE",
        "TOK_TRUE",
        "TOK_AND",
        "TOK_AS",
        "TOK_ASSERT",
        "TOK_BREAK",
        "TOK_CLASS",
        "TOK_CONTINUE",
        "TOK_DEF",
        "TOK_DEL",
        "TOK_ELIF",
        "TOK_ELSE",
        "TOK_EXCEPT",
        "TOK_FINALLY",
        "TOK_FOR",
        "TOK_FROM",
        "TOK_GLOBAL",
        "TOK_IF",
        "TOK_IMPORT",
        "TOK_IN",
        "TOK_IS",
        "TOK_LAMBDA",
        "TOK_NONLOCAL",
        "TOK_NOT",
        "TOK_OR",
        "TOK_PASS",
        "TOK_RAISE",
        "TOK_RETURN",
        "TOK_TRY",
        "TOK_WHILE",
        "TOK_WITH",
        "TOK_YIELD",

        //operators 
        "TOK_ADD",
        "TOK_SUB",
        "TOK_MUL",
        "TOK_DIV",
        "TOK_MOD",
        "TOK_EXP",
        "TOK_FLOOR_DIV",
        "TOK_BIT_AND",
        "TOK_BIT_OR",
        "TOK_BIT_XOR",
        "TOK_BIT_NOT",
        "TOK_LEFT_SHIFT",
        "TOK_RIGHT_SHIFT",

        //bool operators 
        "TOK_LESS_THAN",
        "TOK_GREATER_THAN",
        "TOK_LESS_EQUAL",
        "TOK_GREATER_EQUAL",
        "TOK_EQUAL",
        "TOK_NOT_EQUAL",
        "TOK_ASSIGN",

        //assignment operators
        "TOK_ADD_ASSIGN",
        "TOK_SUB_ASSIGN",
        "TOK_MUL_ASSIGN",
        "TOK_DIV_ASSIGN",
        "TOK_MOD_ASSIGN",
        "TOK_EXP_ASSIGN",
        "TOK_FLOOR_DIV_ASSIGN",
        "TOK_LEFT_SHIFT_ASSIGN",
        "TOK_RIGHT_SHIFT_ASSIGN",
        "TOK_BIT_AND_ASSIGN",
        "TOK_BIT_OR_ASSIGN",
        "TOK_BIT_XOR_ASSIGN",

        //containers
        "TOK_LEFT_PAREN",
        "TOK_RIGHT_PAREN",
        "TOK_LEFT_BRACE",
        "TOK_RIGHT_BRACE",
        "TOK_LEFT_BRACKET",
        "TOK_RIGHT_BRACKET",
        "TOK_SQUOTE",
        "TOK_DQUOTE",

        "TOK_TAB",
        "TOK_NEW_LINE",
        "TOK_COMMA",
        "TOK_COLON",
        "TOK_SEMICOLON",
        "TOK_DOT",
        "TOK_ELLIPSIS",
        "TOK_AT",

        "TOK_IDENTIFIER",
        "TOK_INTEGER",
        "TOK_FLOAT",
        "TOK_STRING",

        "TOK_UNKOWN",
    };

    void print_token(Token token){
        if(token.type == TOK_STRING || token.type == TOK_IDENTIFIER || token.type == TOK_INTEGER || token.type == TOK_FLOAT){
            printf("%s -> %s\n", token_names[token.type], token.literal.str);
        } else{
            printf("%s\n", token_names[token.type]);
        }
    }
#else
    void print_token(Token token) {};
#endif 

Tokenizer tokenizer = {0};


void tokenizer_create(const char* file_name){
    File f = file_open(file_name); 
    array_list_create(tokenizer.tokens, Token);
    tokenizer.line = 1;
    tokenizer.file = f;
}


void token_delete(Token* t){
    t->type = TOK_UNKOWN;
    string_delete(&t->literal);
}

void delete_tokenizer(){
    for (int i = 0; i < tokenizer.tokens.size; i++) {
        Token curr_tok = array_list_get(tokenizer.tokens, Token, i); 
        token_delete(&curr_tok);
    }
    array_list_delete(tokenizer.tokens); 
}

TokenType get_prev_token(){
    if(tokenizer.tokens.size != 0) {
        Token t = array_list_get(tokenizer.tokens, Token, tokenizer.tokens.size - 1);
        return t.type;
    }
    return TOK_UNKOWN;
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

void tokenize_file(){

    //holds a string, keyword, float, or int 
    String curr_literal = string_create();

    Stack enclosure_stack;
    stack_create(&enclosure_stack);
    bool is_string = false;


    while(true) {
        char c = file_next_char(&tokenizer.file); 

        if(is_string && c != '\n' && c != EOF){
            if(c == '\"'){
                validate_enclosures(&enclosure_stack, c); 
                add_token_with_string(TOK_STRING, &curr_literal);
                add_token(TOK_DQUOTE);
                string_clear(&curr_literal);
                is_string = false;
            } else {
                string_push(&curr_literal, c);
            }

            continue;
        }

        switch (c) {
            case '=':
                switch (file_peek(&tokenizer.file)) {
                    case '=':
                        add_token_check_misc(TOK_EQUAL, &curr_literal);
                        break;
                    default:
                        add_token_check_misc(TOK_ASSIGN, &curr_literal);
                }
                break;  
            case '\n':
                if(get_prev_token() == TOK_NEW_LINE) break;
                tokenizer.line++;
                add_token_check_misc(TOK_NEW_LINE, &curr_literal);
                break;
            case '\t':
                add_token_check_misc(TOK_TAB, &curr_literal);
                break;
            case '(':
                stack_push(&enclosure_stack, c);
                add_token_check_misc(TOK_LEFT_PAREN, &curr_literal);
                break; 
            case ')':
                validate_enclosures(&enclosure_stack, c);
                add_token_check_misc(TOK_RIGHT_PAREN, &curr_literal);
                break; 
            case ':':
                add_token_check_misc(TOK_COLON, &curr_literal);
                break;
            case ',':
                add_token_check_misc(TOK_COMMA, &curr_literal);
                break;
            case '+':
                switch (file_peek(&tokenizer.file)) {
                    case '=':
                        add_token_check_misc(TOK_ADD_ASSIGN, &curr_literal);
                    default:
                        add_token_check_misc(TOK_ADD, &curr_literal);
                }
                break;
            case '-':
                switch (file_peek(&tokenizer.file)) {
                    case '=':
                        add_token_check_misc(TOK_SUB_ASSIGN, &curr_literal);
                    default:
                        add_token_check_misc(TOK_SUB_ASSIGN, &curr_literal);
                }
                break;
            case '*': {
                switch (file_peek(&tokenizer.file)) {
                    case '*':
                      add_token_check_misc(TOK_EXP, &curr_literal);
                      file_next_char(&tokenizer.file);
                      break;
                    case '=':
                      add_token_check_misc(TOK_MUL_ASSIGN, &curr_literal);
                      file_next_char(&tokenizer.file);
                      break;
                    default:
                        add_token_check_misc(TOK_MUL, &curr_literal);
              }
                break;
            }
            case '!':
                printf("Todo implemented not equal");
                break;

            case '/': {
                      switch (file_peek(&tokenizer.file)) {
                          case '/':
                              add_token_check_misc(TOK_FLOOR_DIV, &curr_literal);
                              file_next_char(&tokenizer.file);
                              break;
                          case '=':
                              add_token_check_misc(TOK_DIV_ASSIGN, &curr_literal);
                              file_next_char(&tokenizer.file);
                              break;
                          default:
                              add_token_check_misc(TOK_DIV, &curr_literal);
                      }
                      break;
                  }
            case '%':
                      switch (file_peek(&tokenizer.file)) {
                          case '=':
                              add_token_check_misc(TOK_MOD_ASSIGN, &curr_literal);
                              break;
                          default:
                              add_token_check_misc(TOK_MOD, &curr_literal); 
                      }
                      break;
            case '<': {
                      switch (file_peek(&tokenizer.file)) {
                          case '=':
                              add_token_check_misc(TOK_LESS_EQUAL, &curr_literal);
                              file_next_char(&tokenizer.file);
                              break;
                          case '<':
                              add_token_check_misc(TOK_LEFT_SHIFT, &curr_literal);
                              file_next_char(&tokenizer.file);
                              break;
                          default:
                              add_token_check_misc(TOK_LESS_THAN, &curr_literal);
                      }
                      break;
                  }
            case '>': {
                      switch (file_peek(&tokenizer.file)) {
                          case '=':
                              add_token_check_misc(TOK_GREATER_EQUAL, &curr_literal);
                              file_next_char(&tokenizer.file);
                              break;
                          case '>':
                              add_token_check_misc(TOK_RIGHT_SHIFT, &curr_literal);
                              file_next_char(&tokenizer.file);
                              break;
                          default:
                              add_token_check_misc(TOK_GREATER_THAN, &curr_literal); 
                      }
                      break;
                  }
            case '\"':
                      is_string = true;
                      stack_push(&enclosure_stack, c);
                      add_token_check_misc(TOK_DQUOTE, &curr_literal);
                      break;
            case '\'':
                      add_token_check_misc(TOK_SQUOTE, &curr_literal);
                      break;
            case '[': 
                      stack_push(&enclosure_stack, c);
                      add_token_check_misc(TOK_LEFT_BRACKET, &curr_literal);
                      break;
            case ']': 
                      validate_enclosures(&enclosure_stack, c);
                      add_token_check_misc(TOK_RIGHT_BRACKET, &curr_literal);
                      break;
            case '{':   
                    stack_push(&enclosure_stack, c);
                    add_token_check_misc(TOK_LEFT_BRACE, &curr_literal);
                    break;
            case '}':
                    validate_enclosures(&enclosure_stack, c);
                    add_token_check_misc(TOK_RIGHT_BRACE, &curr_literal);
                    break;
            case '.':
                      add_token_check_misc(TOK_DOT, &curr_literal);
                      break;
            case '&':
                      add_token_check_misc(TOK_BIT_AND, &curr_literal);
                      break;
            case '|':
                      add_token_check_misc(TOK_BIT_OR, &curr_literal);
                      break;
            case '^':
                      add_token_check_misc(TOK_BIT_XOR, &curr_literal);
                      break;
            case '~':
                      add_token_check_misc(TOK_BIT_NOT, &curr_literal);
                      break;

                      /* MISSING missing comments, @, ;, bitwise ops assign, exp assign and //= */ 
            default:
                if(isalnum(c)){
                    string_push(&curr_literal, c);
                }
                else if(isspace(c) || c == EOF){
                    if(curr_literal.size != 0) {
                        check_misc_string(&curr_literal);
                        string_clear(&curr_literal);
                    } 
                    else if(c == EOF){
                        goto END;
                    }
                } else{
                    fprintf(stderr, "Invalid token %c on line %d\n", c, tokenizer.line);
                    exit(2);
                }
        }

    }
END: 
    for(int i =0; i < tokenizer.tokens.size; i++){
        Token curr_tok = array_list_get(tokenizer.tokens, Token, i);
        print_token(curr_tok); 
    }

    delete_tokenizer();
}






