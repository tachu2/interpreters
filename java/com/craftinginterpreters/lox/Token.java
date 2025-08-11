package com.craftinginterpreters.lox;

/**
 * A token is a lexical unit in the source code.
 * It is a pair of a TokenType and a lexeme.
 * The lexeme is the actual text of the token.
 * The TokenType is the type of the token.
 * The literal is the value of the token.
 * The line is the line number of the token in the source code.
 */
public class Token {
    final TokenType type;
    final String lexeme;
    final Object literal;
    final int line;
    
    Token(TokenType type, String lexeme, Object literal, int line) {
        this.type = type;
        this.lexeme = lexeme;
        this.literal = literal;
        this.line = line;
    }

    public String toString() {
        return type + " " + lexeme + " " + literal;
    }
}
