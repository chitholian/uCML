/*
   Copyright 2019 Atikur Rahman Chitholian

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#ifndef UCML_NODES_H
#define UCML_NODES_H

#include <vector>
#include <llvm/IR/Value.h>
#include <llvm/IR/Module.h>
#include <string>
#include "context.hpp"


struct YYLTYPE
{
    int         first_line{0};
    int         first_column{0};
    int         last_line{0};
    int         last_column{0};
};


namespace ucml {

    class Node {
    public:
        virtual llvm::Value *generateCode(Context &context);

        virtual ~Node() = default;
    };

    class Statement : public Node {
    };

    class Expression : public Statement {
    };

    class Identifier : public Expression {
    public:
        YYLTYPE location;
        const std::string &name;

        explicit Identifier(YYLTYPE location, const std::string &name);

        llvm::Value *generateCode(Context &context) override;
    };

    class VariableDeclaration : public Statement {
    public:
        YYLTYPE location;
        const Identifier &type;
        Identifier &identifier;
        Expression *expression;

        VariableDeclaration(YYLTYPE location, const Identifier &type, Identifier &name, Expression *expr = nullptr);

        llvm::Value *generateCode(Context &context) override;
    };

    typedef std::vector<Statement *> StatementList;
    typedef std::vector<Expression *> ExpressionList;
    typedef std::vector<VariableDeclaration *> VariableList;

    class Block : public Node {
    public:
        StatementList statements;

        llvm::Value *generateCode(Context &context) override;
    };


    class ExprStatement : public Statement {
    public:
        Expression &expression;

        explicit ExprStatement(Expression &expr);

        llvm::Value *generateCode(Context &context) override;
    };

    class Integer : public Expression {
    public:
        long long value;

        explicit Integer(long long value);

        llvm::Value *generateCode(Context &context) override;
    };

    class Double : public Expression {
    public:
        double value;

        explicit Double(double value);

        llvm::Value *generateCode(Context &context) override;
    };

    class BinaryOperation : public Expression {
    public:
        YYLTYPE location;
        int operation;
        Expression &left, &right;

        BinaryOperation(YYLTYPE location, int op, Expression &lhs, Expression &rhs);

        llvm::Value *generateCode(Context &context) override;
    };

    class UnaryOperation : public Expression {
    public:
        YYLTYPE location;
        int operation;
        Expression &expression;

        UnaryOperation(YYLTYPE location, int op, Expression &expr);

        llvm::Value *generateCode(Context &context) override;
    };

    class Assignment : public Expression {
    public:
        YYLTYPE location;
        Identifier &identifier;
        Expression &expression;

        Assignment(YYLTYPE location, Identifier &id, Expression &expr);

        llvm::Value *generateCode(Context &context) override;
    };

    class FunctionDeclaration : public Statement {
    public:
        YYLTYPE location;
        const Identifier &type, &identifier;
        Block *body;
        VariableList *parameters;
        bool isExternal;

        FunctionDeclaration(YYLTYPE location, const Identifier &type, const Identifier &name, Block *body = nullptr,
                            VariableList *params = nullptr, bool isExt = false);

        llvm::Value *generateCode(Context &context) override;
    };

    class FunctionCall : public Expression {
    public:
        YYLTYPE location;
        const Identifier &identifier;
        ExpressionList *args;

        explicit FunctionCall(YYLTYPE location, const Identifier &name, ExpressionList *args = nullptr);

        llvm::Value *generateCode(Context &context) override;
    };

    class ForLoop : public Statement {
    public:
        Identifier &name;
        const Identifier &type;
        Expression &from, &to;
        Block &body;
        Expression *by;

        ForLoop(Identifier &varName, const Identifier &type, Expression &from, Expression &to, Block &body,
                Expression *by = nullptr);

        llvm::Value *generateCode(Context &context) override;
    };

    class IfCondition : public Statement {
    public:
        YYLTYPE location;
        Expression &condition;
        Block &thenBlock, *elseBlock;

        IfCondition(YYLTYPE location, Expression &cond, Block &thenBlock, Block *elseBlock = nullptr);

        llvm::Value *generateCode(Context &context) override;
    };

    class ReturnStatement : public Statement {
    public:
        YYLTYPE location;
        Expression *expression;

        explicit ReturnStatement(YYLTYPE location, Expression *expr = nullptr);

        llvm::Value *generateCode(Context &context) override;
    };
}
#endif
