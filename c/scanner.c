#include "scanner.h"
#include <ctype.h>
#include <string.h>

typedef struct {
    const char* start;
    const char* current; // the next character to be scanned
    int line;
} Scanner;

Scanner scanner;

void initScanner(const char* source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

static bool isAtEnd() {
    return *scanner.current == '\0';
}

/**
 * Advances the current character and returns the next character to be scanned.
 * @return the next character to be scanned.
 */
static char advance() {
    scanner.current++;
    return scanner.current[-1];
}

static char peek() {
    return *scanner.current;
}

static char peekNext() {
    if (isAtEnd()) {
        return '\0';
    }
    return scanner.current[1];
}

/**
 * Checks if the next character to be scanned matches the expected character.
 * If it does, advances the current character and returns true.
 * If it does not, returns false.
 * @param expected the expected character.
 * @return true if the next character to be scanned matches the expected character, false otherwise.
 */
static bool match(char expected) {
    if (isAtEnd()) {
        return false;
    }
    if (*scanner.current != expected) {
        return false;
    }
    scanner.current++;
    return true;
}

static Token makeToken(TokenType type) {
    Token token = {type, scanner.start, (int)(scanner.current - scanner.start), scanner.line};
    return token;
}

static Token errorToken(const char* message) {
    Token token = {TOKEN_ERROR, message, (int)strlen(message), scanner.line};
    return token;
}

static void skipWhitespace() {
    for (;;) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n': {
                scanner.line++;
                advance();
                break;
            }
            case '/': {
                if (peekNext() != '/') {
                    return;
                }
                while (peek() != '\n' && !isAtEnd()) {
                    advance();
                }
                break;
            }
            default: {
                return;
            }
        }
    }
}

static Token number() {
    while (isdigit((int)peek())) {
        advance();
    }

    if (peek() == '.' && isdigit((int)peekNext())) {
        // consume the '.'
        advance();
        while (isdigit((int)peek())) {
            advance();
        }
    }

    return makeToken(TOKEN_NUMBER);
}

static Token string() {
    while (peek() != '"' && !isAtEnd()) {
        advance();
    }
    if (isAtEnd()) {
        return errorToken("Unterminated string.");
    }

    // remove the leading and trailing quotes
    advance();
    return makeToken(TOKEN_STRING);
}

Token scanToken() {
    skipWhitespace();
    scanner.start = scanner.current;

    if (isAtEnd()) {
        return makeToken(TOKEN_EOF);
    }

    char c = advance();
    if (isdigit((int)c)) {
        return number();
    }

    switch (c) {
        case '(': {
            return makeToken(TOKEN_LEFT_PAREN);
        }
        case ')': {
            return makeToken(TOKEN_RIGHT_PAREN);
        }
        case '{': {
            return makeToken(TOKEN_LEFT_BRACE);
        }
        case '}': {
            return makeToken(TOKEN_RIGHT_BRACE);
        }
        case ';': {
            return makeToken(TOKEN_SEMICOLON);
        }
        case ',': {
            return makeToken(TOKEN_COMMA);
        }
        case '.': {
            return makeToken(TOKEN_DOT);
        }
        case '-': {
            return makeToken(TOKEN_MINUS);
        }
        case '+': {
            return makeToken(TOKEN_PLUS);
        }
        case '/': {
            return makeToken(TOKEN_SLASH);
        }
        case '*': {
            return makeToken(TOKEN_STAR);
        }
        case '!': {
            return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        }
        case '=': {
            return makeToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        }
        case '<': {
            return makeToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        }
        case '>': {
            return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        }
        case '"': {
            return string();
        }
    }

    return errorToken("Unexpected character.");
}