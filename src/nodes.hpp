#ifndef UCML_NODES_H
#define UCML_NODES_H

#include <vector>
#include <llvm/IR/Value.h>
#include <string>
#include "context.hpp"

namespace ucml {

    class Node {
    public:
        virtual llvm::Value *generateCode(Context& context);

        virtual ~Node() = default;
    };

    class Statement : public Node {
    };

    class Expression : public Statement {
    };

    class Identifier : public Expression {
    public:
        const std::string &name;

        explicit Identifier(const std::string &name);

        llvm::Value *generateCode(Context& context) override;
    };

    class VariableDeclaration : public Statement {
    public:
        const Identifier &type;
        Identifier &name;
        Expression *expression;

        VariableDeclaration(const Identifier &type, Identifier &name, Expression *expr = nullptr);

        llvm::Value *generateCode(Context& context) override;
    };

    typedef std::vector<Statement *> StatementList;
    typedef std::vector<Expression *> ExpressionList;
    typedef std::vector<VariableDeclaration *> VariableList;

    class Block : public Node {
    public:
        StatementList statements;

        llvm::Value *generateCode(Context& context) override;
    };


    class ExprStatement : public Statement {
    public:
        Expression &expression;

        explicit ExprStatement(Expression &expr);

        llvm::Value *generateCode(Context& context) override;
    };

    class Integer : public Expression {
    public:
        long long value;

        explicit Integer(long long value);

        llvm::Value *generateCode(Context& context) override;
    };

    class Double : public Expression {
    public:
        double value;

        explicit Double(double value);

        llvm::Value *generateCode(Context& context) override;
    };

    class BinaryOperation : public Expression {
    public:
        int operation;
        Expression &left, &right;

        BinaryOperation(int op, Expression &lhs, Expression &rhs);

        llvm::Value *generateCode(Context& context) override;
    };

    class UnaryOperation : public Expression {
    public:
        int operation;
        Expression &expression;

        UnaryOperation(int op, Expression &expr);

        llvm::Value *generateCode(Context& context) override;
    };

    class Assignment : public Expression {
    public:
        Identifier &identifier;
        Expression &expression;

        Assignment(Identifier &id, Expression &expr);

        llvm::Value *generateCode(Context& context) override;
    };

    class FunctionDeclaration : public Statement {
    public:
        const Identifier &type, &name;
        Block &body;
        VariableList *parameters;

        FunctionDeclaration(const Identifier &type, const Identifier &name, Block &body,
                            VariableList *params = nullptr);

        llvm::Value *generateCode(Context& context) override;
    };

    class FunctionCall : public Expression {
    public:
        const Identifier &name;
        ExpressionList *args;

        explicit FunctionCall(const Identifier &name, ExpressionList *args = nullptr);

        llvm::Value *generateCode(Context& context) override;
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

        llvm::Value *generateCode(Context& context) override;
    };

    class IfCondition : public Statement {
    public:
        Expression &condition;
        Block &thenBlock, *elseBlock;

        IfCondition(Expression &cond, Block &thenBlock, Block *elseBlock = nullptr);

        llvm::Value *generateCode(Context& context) override;
    };

    class ReturnStatement : public Statement {
    public:
        Expression &expression;

        explicit ReturnStatement(Expression &expr);

        llvm::Value *generateCode(Context& context) override;
    };

    class ExternalVariable : public Statement {
    public:
        const Identifier &type;
        Identifier &name;

        ExternalVariable(const Identifier &type, Identifier &name);

        llvm::Value *generateCode(Context& context) override;
    };

    class ExternalFunction : public Statement {
    public:
        const Identifier &type, &name;
        VariableList *parameters;

        ExternalFunction(const Identifier &type, const Identifier &name, VariableList *params = nullptr);

        llvm::Value *generateCode(Context& context) override;
    };
}
#endif
