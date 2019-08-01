#include <iostream>
#include <llvm/IR/Type.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRBuilder.h>
#include "nodes.hpp"
#include "tools.hpp"
#include "parser.hpp"

namespace ucml {

    /*******************************\
    *         Constructors          *
    \*******************************/
    Identifier::Identifier(YYLTYPE location, const std::string &name) : location(location), name(name) {}

    VariableDeclaration::VariableDeclaration(YYLTYPE location, const Identifier &type, Identifier &name,
                                             Expression *expr) : location(location), type(type), identifier(name),
                                                                 expression(expr) {}

    ExprStatement::ExprStatement(Expression &expr) : expression(expr) {}

    Integer::Integer(long long value) : value(value) {}

    Double::Double(double value) : value(value) {}

    BinaryOperation::BinaryOperation(YYLTYPE location, int op, Expression &lhs, Expression &rhs) :
            location(location), operation(op), left(lhs), right(rhs) {}

    UnaryOperation::UnaryOperation(YYLTYPE location, int op, Expression &expr) : location(location), operation(op),
                                                                                 expression(expr) {}

    Assignment::Assignment(YYLTYPE location, Identifier &id, Expression &expr) : location(location), identifier(id),
                                                                                 expression(expr) {}

    FunctionDeclaration::FunctionDeclaration(
            YYLTYPE location, const Identifier &type, const Identifier &name, Block *body, VariableList *params,
            bool isExt) :
            location(location), type(type), identifier(name), body(body), parameters(params), isExternal(isExt) {}

    FunctionCall::FunctionCall(YYLTYPE location, const Identifier &name, ExpressionList *args) : location(location),
                                                                                                 identifier(name),
                                                                                                 args(args) {}

    ForLoop::ForLoop(Identifier &varName, const Identifier &type,
                     Expression &from, Expression &to, Block &body, Expression *by) :
            name(varName), type(type), from(from), to(to), body(body), by(by) {}

    IfCondition::IfCondition(YYLTYPE location, Expression &cond, Block &thenBlock, Block *elseBlock) :
            location(location), condition(cond), thenBlock(thenBlock), elseBlock(elseBlock) {}

    ReturnStatement::ReturnStatement(YYLTYPE location, Expression *expr) : location(location), expression(expr) {}

    /*******************************\
    *       Code Generators         *
    \*******************************/
    llvm::Value *Node::generateCode(Context &context) {
        return nullptr;
    }

    llvm::Value *Identifier::generateCode(Context &context) {
        llvm::Value *idPointer = Tools::getValueOfIdentifier(context, name);
        if (idPointer) {
            return new llvm::LoadInst(idPointer, "", false, context.getCurrentBlock());
        }
        FATAL(location, "Undefined Variable \"" << name << "\"");
        return nullptr;
    }

    llvm::Value *VariableDeclaration::generateCode(Context &context) {
        if (!Tools::isValidType(type.name)) {
            FATAL(location, "Invalid Type \"" << type.name << "\"");
            return nullptr;
        }
        if (context.isEmpty()) {
            context.module->getOrInsertGlobal(identifier.name, Tools::typeOf(identifier, context.llvmContext));
            llvm::GlobalVariable *globalVariable = context.module->getNamedGlobal(identifier.name);
            globalVariable->setLinkage(llvm::GlobalValue::InternalLinkage);
        } else {
            if (context.getSymbols().find(identifier.name) == context.getSymbols().end()) {
                auto *allocationInst = new llvm::AllocaInst(Tools::typeOf(type, context.llvmContext),
                                                            0, identifier.name, context.getCurrentBlock());
                context.getSymbols()[identifier.name] = allocationInst;
            } else {
                FATAL(location, "Variable \"" << identifier.name << "\" is already defined.");
                return nullptr;
            }
        }

        if (expression) {
            return (new Assignment(identifier.location, identifier, *expression))->generateCode(context);
        }
        return nullptr;
    }

    llvm::Value *Block::generateCode(Context &context) {
        llvm::Value *lastStatement = nullptr;
        for (auto &statement : statements) {
            lastStatement = statement->generateCode(context);
        }
        return lastStatement;
    }

    llvm::Value *Integer::generateCode(Context &context) {
        return llvm::ConstantInt::get(llvm::Type::getInt64Ty(context.llvmContext), value, true);
    }

    llvm::Value *Double::generateCode(Context &context) {
        return llvm::ConstantFP::get(llvm::Type::getDoubleTy(context.llvmContext), value);
    }

    llvm::Value *ExprStatement::generateCode(Context &context) {
        return expression.generateCode(context);
    }

    llvm::Value *BinaryOperation::generateCode(Context &context) {
        llvm::Value *leftValue = left.generateCode(context), *rightValue = right.generateCode(context);
        if (!(leftValue && rightValue)) return nullptr;

        llvm::IRBuilder<> irBuilder(context.getCurrentBlock());
        llvm::Instruction::BinaryOps op;
        switch (operation) {
            // todo: type check.
            case '+':
                op = llvm::Instruction::Add;
                break;
            case '-':
                op = llvm::Instruction::Sub;
                break;
            case '*':
                op = llvm::Instruction::Mul;
                break;
            case '/':
                op = llvm::Instruction::SDiv;
                break;
            case '%':
                op = llvm::Instruction::SRem;
                break;
            case EQ:
                return irBuilder.CreateICmpEQ(leftValue, rightValue, "eq");
            case NE:
                return irBuilder.CreateICmpNE(leftValue, rightValue, "ne");
            case LT:
                return irBuilder.CreateICmpSLT(leftValue, rightValue, "lt");
            case GT:
                return irBuilder.CreateICmpSGT(leftValue, rightValue, "gt");
            case LE:
                return irBuilder.CreateICmpSLE(leftValue, rightValue, "le");
            case GE:
                return irBuilder.CreateICmpSGE(leftValue, rightValue, "ge");
            default:
                return nullptr;
        }
        return llvm::BinaryOperator::Create(op, leftValue, rightValue, "", context.getCurrentBlock());
    }

    llvm::Value *UnaryOperation::generateCode(Context &context) {
        return nullptr;
    }

    llvm::Value *Assignment::generateCode(Context &context) {
        auto it = Tools::getValueOfIdentifier(context, identifier.name);
        if (!it) {
            FATAL(location, "Undeclared variable \"" << identifier.name << "\"");
            return nullptr;
        }
        llvm::Value *value = expression.generateCode(context);

        if (!value) {
            FATAL(location, "Invalid assignment operation.");
            return nullptr;
        }
        new llvm::StoreInst(value, Tools::getValueOfIdentifier(context, identifier.name), false,
                            context.getCurrentBlock());
        return value;
    }

    llvm::Value *FunctionDeclaration::generateCode(Context &context) {
        if (!Tools::isValidType(type.name, true)) {
            FATAL(location, "Invalid return type \"" << type.name << "\".");
            return nullptr;
        }
        if (context.module->getFunction(identifier.name)) {
            FATAL(location, "Function with name \"" << identifier.name << "\" is already defined.");
            return nullptr;
        }
        std::vector<llvm::Type *> argTypes;
        if (parameters) {
            for (auto &arg : *parameters) {
                if (Tools::isValidType(arg->type.name))
                    argTypes.push_back(Tools::typeOf(arg->type, context.llvmContext));
                else {
                    FATAL(location, "Invalid Parameter Type \"" << arg->type.name << "\"");
                    return nullptr;
                }
            }
        }

        llvm::FunctionType *functionType = llvm::FunctionType::get(Tools::typeOf(type, context.llvmContext),
                                                                   llvm::makeArrayRef(argTypes), false);
        llvm::Function *function = llvm::Function::Create(functionType,
                                                          (isExternal ? llvm::GlobalValue::ExternalLinkage
                                                                      : llvm::GlobalValue::InternalLinkage),
                                                          identifier.name, context.module);
        function->setCallingConv(llvm::CallingConv::C);
        if (isExternal)
            return function;
        llvm::BasicBlock *basicBlock = llvm::BasicBlock::Create(context.llvmContext, "entry", function, nullptr);
        context.pushBlock(basicBlock);
        llvm::Function::arg_iterator iterator = function->arg_begin();
        llvm::Value *argValue;
        if (parameters) {
            for (auto &arg : *parameters) {
                argValue = &*iterator++;
                arg->generateCode(context);
                new llvm::StoreInst(argValue, Tools::getValueOfIdentifier(context, arg->identifier.name), false,
                                    basicBlock);
            }
        }
        body->generateCode(context);
        bool misMatch = false;
        if (context.getReturnValue() && type.name == "void") {
            misMatch = true;
        } else if (context.getReturnValue() &&
                   Tools::typeOf(type, context.llvmContext) != context.getReturnValue()->getType()) {
            misMatch = true;
        } else if (!context.getReturnValue() && type.name != "void") {
            misMatch = true;
        }
        if (misMatch) {
            FATAL(type.location, "Return-type does not match the type of returned value.");
            return nullptr;
        }
        llvm::ReturnInst::Create(context.llvmContext, context.getReturnValue(), context.getCurrentBlock());

        context.popBlock();
        return function;
    }

    llvm::Value *FunctionCall::generateCode(Context &context) {
        llvm::Function *function = context.module->getFunction(identifier.name);
        if (!function) {
            FATAL(location, "Undefined function \"" << identifier.name << "\"");
            return nullptr;
        }
        std::vector<llvm::Value *> arguments;
        if (args) {
            if (function->arg_size() < args->size()) {
                FATAL(location,
                      "Function \"" << identifier.name << "\" accepts only " << function->arg_size() << " argument" <<
                                    (function->arg_size() > 1 ? "s" : "") << " but " << args->size()
                                    << (args->size() > 1 ? " were" : " was") << " given.");
                return nullptr;
            }
            for (auto &arg : *args) {
                arguments.push_back((*arg).generateCode(context));
            }
        }
        return llvm::CallInst::Create(function, llvm::makeArrayRef(arguments), "",
                                      context.getCurrentBlock());
    }

    llvm::Value *ForLoop::generateCode(Context &context) {
        return nullptr;
    }

    llvm::Value *IfCondition::generateCode(Context &context) {
        llvm::Value *conditionValue = condition.generateCode(context);
        if (!conditionValue) {
            FATAL(location, "Invalid condition given to \"if\" statement.");
            return nullptr;
        }
        llvm::Function *function = context.getCurrentBlock()->getParent();
        llvm::BasicBlock
                *then = llvm::BasicBlock::Create(context.llvmContext, "then", function),
                *otherwise = llvm::BasicBlock::Create(context.llvmContext, "otherwise", function),
                *merge = llvm::BasicBlock::Create(context.llvmContext, "merge", function);
        llvm::BranchInst::Create(then, otherwise, conditionValue, context.getCurrentBlock());
        context.pushBlock(then);
        thenBlock.generateCode(context);
        llvm::BranchInst::Create(merge, otherwise, conditionValue, context.getCurrentBlock());
        context.popBlock();

        context.pushBlock(otherwise);
        if (elseBlock) {
            elseBlock->generateCode(context);
        }
        llvm::BranchInst::Create(merge, context.getCurrentBlock());
        context.popBlock();
        context.pushBlock(merge);

        return merge;
    }

    llvm::Value *ReturnStatement::generateCode(Context &context) {
        // todo: check if outside a function.
        if (expression) {
            context.setReturnValue(expression->generateCode(context));
            return context.getReturnValue();
        }
        return nullptr;
    }
}
