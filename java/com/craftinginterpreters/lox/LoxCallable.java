package com.craftinginterpreters.lox;

import java.util.List;

public interface LoxCallable {
    int arity(); // number of arguments
    Object call(Interpreter interpreter, List<Object> arguments);
}
