#include <iostream>
#include "nodes.hpp"

namespace ucml {

    /*******************************\
    *         Constructors          *
    \*******************************/
    Identifier::Identifier(const std::string &name) : name(name) {}

    VariableDeclaration::VariableDeclaration(const Identifier &type, Identifier &name,
            Expression *expr) : type(type), name(name), expression(expr) {
    }

    ExprStatement::ExprStatement(Expression &expr) : expression(expr) {}

    Integer::Integer(long long value) : value(value) {}

    Double::Double(double value) : value(value) {}

    BinaryOperation::BinaryOperation(int op, Expression &lhs, Expression &rhs) :
            operation(op), left(lhs), right(rhs) {}

    UnaryOperation::UnaryOperation(int op, Expression &expr) : operation(op), expression(expr) {}

    Assignment::Assignment(Identifier &id, Expression &expr) : identifier(id), expression(expr) {}

    FunctionDeclaration::FunctionDeclaration(
            const Identifier &type, const Identifier &name, Block &body,
            VariableList *params) : type(type), name(name), body(body), parameters(params) {}

    FunctionCall::FunctionCall(const Identifier &name, ExpressionList *args) : name(name), args(args) {}

    ForLoop::ForLoop(Identifier &varName, const Identifier &type,
                     Expression &from, Expression &to, Block &body, Expression *by) :
            name(varName), type(type), from(from), to(to), body(body), by(by) {}

    IfCondition::IfCondition(Expression &cond, Block &thenBlock, Block *elseBlock) :
            condition(cond), thenBlock(thenBlock), elseBlock(elseBlock) {}

    ReturnStatement::ReturnStatement(Expression &expr) : expression(expr) {}

    ExternalVariable::ExternalVariable(const Identifier &type, Identifier &name) : type(type), name(name) {}

    ExternalFunction::ExternalFunction(
            const Identifier &type, const Identifier &name, VariableList *params) :
            type(type), name(name), parameters(params) {}

    /*******************************\
    *       Code Generators         *
    \*******************************/
    llvm::Value *Node::generateCode(Context& context) {
        return nullptr;
    }

    llvm::Value *Identifier::generateCode(Context& context) {
        return nullptr;
    }

    llvm::Value *VariableDeclaration::generateCode(Context& context) {
        return nullptr;
    }

    llvm::Value *Block::generateCode(Context& context) {
        return nullptr;
    }

    llvm::Value *Integer::generateCode(Context& context) {
        return nullptr;
    }

    llvm::Value *Double::generateCode(Context& context) {
        return nullptr;
    }

    llvm::Value *ExprStatement::generateCode(Context& context) {
        return nullptr;
    }

    llvm::Value *BinaryOperation::generateCode(Context& context) {
        return nullptr;
    }

    llvm::Value *UnaryOperation::generateCode(Context& context) {
        return nullptr;
    }

    llvm::Value *Assignment::generateCode(Context& context) {
        return nullptr;
    }

    llvm::Value *FunctionDeclaration::generateCode(Context& context) {
        return nullptr;
    }

    llvm::Value *FunctionCall::generateCode(Context& context) {
        return nullptr;
    }

    llvm::Value *ForLoop::generateCode(Context& context) {
        return nullptr;
    }

    llvm::Value *IfCondition::generateCode(Context& context) {
        return nullptr;
    }

    llvm::Value *ReturnStatement::generateCode(Context& context) {
        return nullptr;
    }

    llvm::Value *ExternalVariable::generateCode(Context& context) {
        return nullptr;
    }

    llvm::Value *ExternalFunction::generateCode(Context& context) {
        return nullptr;
    }
}
