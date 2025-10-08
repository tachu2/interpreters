#include "compiler.h"
#include "scanner.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    Token current; // the next token to be parsed
    Token previous; // the previous token parsed
    bool hadError;
    bool panicMode;
} Parser;

/**
 * 優先順位
 * 
 * 優先順位は、式の評価順序を決定するためのものです。
 * 優先順位が高いほど、式の評価順序が高くなります。
 */
typedef enum {
  PREC_NONE,
  PREC_ASSIGNMENT,  // =
  PREC_OR,          // or
  PREC_AND,         // and
  PREC_EQUALITY,    // == !=
  PREC_COMPARISON,  // < > <= >=
  PREC_TERM,        // + -
  PREC_FACTOR,      // * /
  PREC_UNARY,       // ! -
  PREC_CALL,        // . ()
  PREC_PRIMARY,     // true false nil this
} Precedence;

typedef void (*ParseFn)();

/**
 * パースルール表
 * 
 */
typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

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

static uint8_t emitConstant(Value value) {
    int constant = addConstant(currentChunk(), value);
    if (constant > UINT8_MAX) {
        error("Too many constants in one chunk.");
        return 0;
    }
    return (uint8_t)constant;
}

static void endCompiler() {
    emitReturn();
#ifdef DEBUG_PRINT_CODE
    if (!parser.hadError) {
        disassembleChunk(currentChunk(), "code");
    }
#endif
}

static void expression();
static ParseRule* getRule(TokenType type);
static void parsePrecedence(Precedence precedence);

/**
 * 二項演算子: + - * /
 * 
 * 二項演算子は、二つのオペランド（被演算子）に対して作用する演算子です（例：1 + 2, x < y）。
 * 先に右のオペランドをコンパイルしてから二項演算子をコンパイルする
 */
static void binary() {
    TokenType operatorType = parser.previous.type;
    ParseRule* rule = getRule(operatorType);
    // 右のオペランドの優先順位を1つ上げる
    // １つ目の+と次の+の優先順位の比較のため
    // 1 + 2 + 3 + 4 -> ((1 + 2) + 3) + 4
    parsePrecedence(rule->precedence + 1);

    switch (operatorType) {
        case TOKEN_BANG_EQUAL: {
            emitBytes(OP_EQUAL, OP_NOT);
            break;
        }
        case TOKEN_EQUAL_EQUAL: {
            emitByte(OP_EQUAL);
            break;
        }
        case TOKEN_GREATER: {
            emitByte(OP_GREATER);
            break;
        }
        case TOKEN_GREATER_EQUAL: {
            // a >= b
            // !(b < a)
            emitBytes(OP_LESS, OP_NOT);
            break;
        }
        case TOKEN_LESS: {
            emitByte(OP_LESS);
            break;
        }
        case TOKEN_LESS_EQUAL: {
            // a <= b
            // !(b > a)
            emitBytes(OP_GREATER, OP_NOT);
            break;
        }
        case TOKEN_PLUS: {
            emitByte(OP_ADD);
            break;
        }
        case TOKEN_MINUS: {
            emitByte(OP_SUBTRACT);
            break;
        }
        case TOKEN_STAR: {
            emitByte(OP_MULTIPLY);
            break;
        }
        case TOKEN_SLASH: {
            emitByte(OP_DIVIDE);
            break;
        }
        default:
            return;
    }
}

static void literal() {
    switch (parser.previous.type) {
        case TOKEN_FALSE: {
            emitByte(OP_FALSE);
            break;
        }
        case TOKEN_TRUE: {
            emitByte(OP_TRUE);
            break;
        }
        case TOKEN_NIL: {
            emitByte(OP_NIL);
            break;
        }
        default:
            return;
    }
}

static void grouping() {
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

static void number() {
    double value = strtod(parser.previous.start, NULL);
    uint8_t constant = emitConstant(NUMBER_VAL(value));
    emitBytes(OP_CONSTANT, constant);
}


/**
 * 単項演算子（unary operator）
 * 
 * 単項演算子は、単一のオペランド（被演算子）に対して作用する演算子です（例：-1, !true）。
 * オペランドを評価して、その値をスタックに残す。
 * その値をポップして、逆転し、その結果をスタックにプッシュする。
 * なのでexpressionを呼び出したあとにunaryの命令を書く
 */
static void unary() {
    TokenType operatorType = parser.previous.type;

    // ex) -1.2 + 3;
    // expressionは1.2 + 3を含めてしまうが、1.2だけを対象にしたい
    parsePrecedence(PREC_UNARY);

    switch (operatorType) {
        case TOKEN_BANG: {
            emitByte(OP_NOT);
            break;
        }
        case TOKEN_MINUS: {
            emitByte(OP_NEGATE);
            break;
        }
        default:
            return;
    }
}

ParseRule rules[] = {
  [TOKEN_LEFT_PAREN]    = {grouping, NULL,   PREC_NONE},
  [TOKEN_RIGHT_PAREN]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LEFT_BRACE]    = {NULL,     NULL,   PREC_NONE}, 
  [TOKEN_RIGHT_BRACE]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_COMMA]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_DOT]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_MINUS]         = {unary,    binary, PREC_TERM},
  [TOKEN_PLUS]          = {NULL,     binary, PREC_TERM},
  [TOKEN_SEMICOLON]     = {NULL,     NULL,   PREC_NONE},
  [TOKEN_SLASH]         = {NULL,     binary, PREC_FACTOR},
  [TOKEN_STAR]          = {NULL,     binary, PREC_FACTOR},
  [TOKEN_BANG]          = {unary,     NULL,   PREC_NONE},
  [TOKEN_BANG_EQUAL]    = {NULL,     binary,   PREC_EQUALITY},
  [TOKEN_EQUAL]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EQUAL_EQUAL]   = {NULL,     binary,   PREC_EQUALITY},
  [TOKEN_GREATER]       = {NULL,     binary,   PREC_COMPARISON},
  [TOKEN_GREATER_EQUAL] = {NULL,     binary,   PREC_COMPARISON},
  [TOKEN_LESS]          = {NULL,     binary,   PREC_COMPARISON},
  [TOKEN_LESS_EQUAL]    = {NULL,     binary,   PREC_COMPARISON},
  [TOKEN_IDENTIFIER]    = {NULL,     NULL,   PREC_NONE},
  [TOKEN_STRING]        = {NULL,     NULL,   PREC_NONE},
  [TOKEN_NUMBER]        = {number,   NULL,   PREC_NONE},
  [TOKEN_AND]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_CLASS]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_ELSE]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FALSE]         = {literal,     NULL,   PREC_NONE},
  [TOKEN_FOR]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FUN]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_IF]            = {NULL,     NULL,   PREC_NONE},
  [TOKEN_NIL]           = {literal,     NULL,   PREC_NONE},
  [TOKEN_OR]            = {NULL,     NULL,   PREC_NONE},
  [TOKEN_PRINT]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_RETURN]        = {NULL,     NULL,   PREC_NONE},
  [TOKEN_SUPER]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_THIS]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_TRUE]          = {literal,     NULL,   PREC_NONE},
  [TOKEN_VAR]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_WHILE]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_ERROR]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EOF]           = {NULL,     NULL,   PREC_NONE},
};

/**
 * 優先順位に従って式を解析する
 */
static void parsePrecedence(Precedence precedence) {
    advance();
    ParseFn prefixRule = getRule(parser.previous.type)->prefix;
    if (prefixRule == NULL) {
        error("Expect expression.");
        return;
    }
    prefixRule();

    // 常に前後の演算子の優先順位を比較する
    // なぜなら次のtokenが演算子ではない場合は、binaryを呼び出して処理されるから
    while (precedence <= getRule(parser.current.type)->precedence) {
        advance();
        ParseFn infixRule = getRule(parser.previous.type)->infix;
        infixRule();
    }
}

static ParseRule* getRule(TokenType type) {
    return &rules[type];
}

/**
 * 式（expression）の解析とコンパイル
 * 
 * 式は値を生成する構文要素です（例：1 + 2, x, f()）。
 * statement（文）とは異なり、式は評価結果を返します。
 * 
 * 文（statement）: 実行する命令（例：if文、while文、変数宣言）
 * 式（expression）: 値を計算する式（例：算術演算、関数呼び出し）
 */
static void expression() {
    parsePrecedence(PREC_ASSIGNMENT);
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