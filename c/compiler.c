#include "compiler.h"
#include "scanner.h"
#include <stdio.h>

typedef struct {
    Token current; // the next token to be parsed
    Token previous; // the previous token parsed
    bool hadError;
    bool panicMode;
} Parser;

Parser parser;
Chunk* compileChunk;

static Chunk* currentChunk() {
    return compileChunk;
}

static void errorAt(Token* token, const char* message) {
    if (parser.panicMode) {
        return;
    }
    fprintf(stderr, "[line %d] Error", token->line);
    if (token->type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    } else if (token->type == TOKEN_ERROR) {
    } else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    parser.hadError = true;
}


static void error(const char* message) {
    errorAt(&(parser.previous), message);

}

static void errorAtCurrent(const char* message) {
    errorAt(&(parser.current), message);
}

static void advance() {
    parser.previous = parser.current;

    for (;;) {
        parser.current = scanToken();
        if (parser.current.type != TOKEN_ERROR) {
            break;
        }

        errorAtCurrent(parser.current.start);
    }
}

/** 
 * Consumes the current token if it matches the given type.
 * @param type The type to check.
 * @param message The error message to throw if the current token does not match the given type.
 */
static void consume(TokenType type, const char* message) {
    if (parser.current.type == type) {
        advance();
        return;
    }

    errorAtCurrent(message);
}

static void emitByte(uint8_t byte) {
    writeChunk(currentChunk(), byte, parser.previous.line);
}

static void emitBytes(uint8_t byte1, uint8_t byte2) {
    emitByte(byte1);
    emitByte(byte2);
}

static void emitReturn() {
    emitByte(OP_RETURN);
}

static void endCompiler() {
    emitReturn();
}

static void expression() {
}

bool compile(const char* source, Chunk *chunk) {
    initScanner(source);
    compileChunk = chunk;
    parser.panicMode = false;
    parser.hadError = false;

    advance();
    expression();
    consume(TOKEN_EOF, "Expect end of expression.");
    endCompiler();
    return !parser.hadError;
}