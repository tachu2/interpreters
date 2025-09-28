#include "scanner.h"
#include <string.h>

typedef struct {
    const char* start;
    const char* current;
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

static Token makeToken(TokenType type) {
    Token token = {type, scanner.start, (int)(scanner.current - scanner.start), scanner.line};
    return token;
}

static Token errorToken(const char* message) {
    Token token = {TOKEN_ERROR, message, (int)strlen(message), scanner.line};
    return token;
}

Token scanToken() {
    scanner.start = scanner.current;

    if (isAtEnd()) {
        return makeToken(TOKEN_EOF);
    }

    return errorToken("Unexpected character.");
}