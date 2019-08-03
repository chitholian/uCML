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
#include <iostream>
#include <llvm/IR/Type.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
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
        auto *idPointer = Tools::getValueOfIdentifier(context, *this);
        if (idPointer && idPointer->second) {
            return llvm::IRBuilder<>(context.getCurrentBlock()).CreateLoad(idPointer->second);
        }
        FATAL(location, "Undefined variable \"" << name << "\"");
        return nullptr;
    }

    llvm::Value *VariableDeclaration::generateCode(Context &context) {
        if (!Tools::isValidType(type.name)) {
            FATAL(location, "Invalid type \"" << type.name << "\"");
            return nullptr;
        }

        if (context.size() <= 1) { // means global scope
            if (context.module->getNamedGlobal(identifier.name)) {
                FATAL(location, "Global variable \"" << identifier.name << "\" is already declared.");
                return nullptr;
            }
            llvm::Type *valueType = Tools::typeOf(type, context.llvmContext);
            llvm::IRBuilder<> builder(context.getCurrentBlock());
            llvm::Constant *defaultValue =
                    type.name == "int" ? builder.getInt64(0) : llvm::ConstantFP::get(builder.getDoubleTy(), 0.0);
            new llvm::GlobalVariable(*context.module, valueType, false, llvm::GlobalValue::InternalLinkage,
                                     defaultValue,
                                     identifier.name);
        } else {
            if (context.getSymbols().find(identifier.name) == context.getSymbols().end()) {
                auto *allocationInst = new llvm::AllocaInst(Tools::typeOf(type, context.llvmContext),
                                                            0, identifier.name, context.getCurrentBlock());
                context.getSymbols()[identifier.name] = std::make_pair(Tools::typeOf(type, context.llvmContext),
                                                                       allocationInst);
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
        bool isFP = false;
        if (leftValue->getType()->getTypeID() == leftValue->getType()->DoubleTyID) {
            isFP = true;
            if (rightValue->getType()->getTypeID() == rightValue->getType()->IntegerTyID) {
                W(location, "Converting integer to double.");
                rightValue = new llvm::SIToFPInst(rightValue, leftValue->getType(), "casted",
                                                  context.getCurrentBlock());
            }

        } else if (rightValue->getType()->getTypeID() == rightValue->getType()->DoubleTyID) {
            isFP = true;
            if (leftValue->getType()->getTypeID() == leftValue->getType()->IntegerTyID) {
                W(location, "Converting integer to double.");
                leftValue = new llvm::SIToFPInst(leftValue, rightValue->getType(), "casted", context.getCurrentBlock());
            }
        }

        llvm::IRBuilder<> irBuilder(context.getCurrentBlock());
        switch (operation) {
            case '+':
                return isFP ? irBuilder.CreateFAdd(leftValue, rightValue) : irBuilder.CreateAdd(leftValue, rightValue);
            case '-':
                return isFP ? irBuilder.CreateFSub(leftValue, rightValue) : irBuilder.CreateSub(leftValue, rightValue);
            case '*':
                return isFP ? irBuilder.CreateFMul(leftValue, rightValue) : irBuilder.CreateMul(leftValue, rightValue);
            case '/':
                return isFP ? irBuilder.CreateFDiv(leftValue, rightValue) : irBuilder.CreateSDiv(leftValue, rightValue);
            case '%':
                return isFP ? irBuilder.CreateFRem(leftValue, rightValue) : irBuilder.CreateSRem(leftValue, rightValue);
            case EQ:
                return isFP ? irBuilder.CreateFCmpOEQ(leftValue, rightValue, "eq") :
                       irBuilder.CreateICmpEQ(leftValue, rightValue, "eq");
            case NE:
                return isFP ? irBuilder.CreateFCmpONE(leftValue, rightValue, "ne") :
                       irBuilder.CreateICmpNE(leftValue, rightValue, "ne");
            case LT:
                return isFP ? irBuilder.CreateFCmpOLT(leftValue, rightValue, "lt") :
                       irBuilder.CreateICmpSLT(leftValue, rightValue, "lt");
            case GT:
                return isFP ? irBuilder.CreateFCmpOGT(leftValue, rightValue, "gt") :
                       irBuilder.CreateICmpSGT(leftValue, rightValue, "gt");
            case LE:
                return isFP ? irBuilder.CreateFCmpOLE(leftValue, rightValue, "le") :
                       irBuilder.CreateICmpSLE(leftValue, rightValue, "le");
            case GE:
                return isFP ? irBuilder.CreateFCmpOGE(leftValue, rightValue, "ge") :
                       irBuilder.CreateICmpSGE(leftValue, rightValue, "ge");
            default:
                return nullptr;
        }
    }

    llvm::Value *UnaryOperation::generateCode(Context &context) {
        llvm::IRBuilder<> builder(context.getCurrentBlock());
        llvm::Value *value = expression.generateCode(context);
        if (!value) {
            FATAL(location, "Invalid operand");
            return nullptr;
        }
        switch (operation) {
            case '-':
                if (value->getType()->getTypeID() == value->getType()->DoubleTyID)
                    return builder.CreateFSub(llvm::ConstantFP::get(builder.getDoubleTy(), 0), value);
                return builder.CreateSub(builder.getInt64(0), value);
        }
        return nullptr;
    }

    llvm::Value *Assignment::generateCode(Context &context) {
        auto destination = Tools::getValueOfIdentifier(context, identifier);
        if (!destination || !destination->second) {
            FATAL(location, "Undeclared variable \"" << identifier.name << "\"");
            return nullptr;
        }
        llvm::Value *value = expression.generateCode(context);

        if (!value) {
            FATAL(location, "Invalid assignment operation.");
            return nullptr;
        }

        llvm::Type::TypeID type = destination->first->getTypeID();

        if (type == llvm::Type::TypeID::IntegerTyID) {
            if (value->getType()->getTypeID() == llvm::Type::TypeID::DoubleTyID) {
                W(location, "Truncating double to fit integer variable.");
                value = new llvm::FPToSIInst(value, destination->first, "casted", context.getCurrentBlock());
            }
        } else if (type == llvm::Type::TypeID::DoubleTyID) {
            if (value->getType()->getTypeID() == llvm::Type::TypeID::IntegerTyID) {
                W(location, "Converting integer to double.");
                value = new llvm::SIToFPInst(value, destination->first, "casted", context.getCurrentBlock());
            }
        }
        new llvm::StoreInst(value, destination->second, context.getCurrentBlock());
        return value;
    }

    llvm::Value *FunctionDeclaration::generateCode(Context &context) {
        if (context.size() > 1) {
            FATAL(location, "Local functions are not supported yet.");
            return nullptr;
        }
        if (!Tools::isValidType(type.name, true)) {
            FATAL(location, "Invalid return type \"" << type.name << "\".");
            return nullptr;
        }
        // Protect our dummy built-in function: echo(number) too!
        if (identifier.name == "echo" || context.module->getFunction(identifier.name)) {
            FATAL(location, "Function with name \"" << identifier.name << "\" is already defined.");
            return nullptr;
        }
        std::vector<llvm::Type *> argTypes;
        if (parameters) {
            for (auto &arg : *parameters) {
                if (Tools::isValidType(arg->type.name))
                    argTypes.push_back(Tools::typeOf(arg->type, context.llvmContext));
                else {
                    FATAL(location, "Invalid parameter type \"" << arg->type.name << "\"");
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
        context.createNewScope(basicBlock);
        llvm::Function::arg_iterator iterator = function->arg_begin();
        llvm::Value *argValue;
        llvm::IRBuilder<> builder(context.getCurrentBlock());
        if (parameters) {
            for (auto &arg : *parameters) {
                argValue = &*iterator++;
                arg->generateCode(context);
                builder.CreateStore(argValue, Tools::getValueOfIdentifier(context, arg->identifier)->second,
                                    false);
            }
        }
        body->generateCode(context);
        builder.SetInsertPoint(context.getCurrentBlock());
        if (!context.getCurrentBlock()->getTerminator()) {
            if (type.name == "void") builder.CreateRetVoid();
            else if (type.name == "int") builder.CreateRet(builder.getInt64(1));
            else builder.CreateRet(llvm::ConstantFP::get(builder.getDoubleTy(), 1.0));
        }
        context.closeCurrentScope();
        return function;
    }

    llvm::Value *FunctionCall::generateCode(Context &context) {
        llvm::Function *function = context.module->getFunction(identifier.name);
        bool notFound = false;
        if (!function) {
            // Check if our dummy "echo()" is called!
            if (identifier.name == "echo") {
                if (!args || args->size() != 1) {
                    FATAL(location, "Function \"echo(number)\" requires exactly one argument.");
                    return nullptr;
                } else {
                    std::vector<llvm::Value *> arguments;
                    llvm::Value *value = (*args->begin())->generateCode(context);
                    if (!value) {
                        FATAL(location, "Invalid argument provided");
                        return nullptr;
                    }
                    arguments.push_back(value);
                    if (value->getType()->getTypeID() == llvm::Type::DoubleTyID) {
                        function = context.module->getFunction("echodouble");
                    } else {
                        function = context.module->getFunction("echoint");
                    }
                    if (!function) {
                        FATAL(location, "Cannot call \"echo()\" function; may be a bug.");
                        return nullptr;
                    }
                    return llvm::IRBuilder<>(context.getCurrentBlock()).
                            CreateCall(function, llvm::makeArrayRef(arguments));
                }
            } else notFound = true;
        }
        if (notFound) {
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
                llvm::Value *value = (*arg).generateCode(context);
                if (!value) {
                    FATAL(location, "Invalid argument provided");
                    return nullptr;
                }
                arguments.push_back(value);
            }
        }
        return llvm::IRBuilder<>(context.getCurrentBlock()).CreateCall(function, llvm::makeArrayRef(arguments));
    }

    llvm::Value *ForLoop::generateCode(Context &context) {
        llvm::Function *function = context.getCurrentBlock()->getParent();
        llvm::BasicBlock
                *initBlock = llvm::BasicBlock::Create(context.llvmContext, "init", function),
                *conditionBlock = llvm::BasicBlock::Create(context.llvmContext, "cond", function),
                *loopBlock = llvm::BasicBlock::Create(context.llvmContext, "loop", function),
                *progressBlock = llvm::BasicBlock::Create(context.llvmContext, "progress", function),
                *afterBlock = llvm::BasicBlock::Create(context.llvmContext, "after", function);
        llvm::BranchInst::Create(initBlock, context.getCurrentBlock());
        context.createNewScope(initBlock);
        llvm::Value *declaration = (new VariableDeclaration(name.location, type, name, &from))->generateCode(context);
        if (!declaration) {
            FATAL(name.location, "Invalid declaration given to loop initialization.");
            return nullptr;
        }
        llvm::BranchInst::Create(conditionBlock, context.getCurrentBlock());
        context.setCurrentBlock(conditionBlock);
        llvm::Value *fromValue = from.generateCode(context), *toValue = to.generateCode(context),
                *idValue = (new Identifier(name.location, name.name))->generateCode(context);
        if (!(fromValue && toValue)) {
            FATAL(name.location, "Invalid range given to \"for\" loop.");
            return nullptr;
        }
        if (type.name != "int" || fromValue->getType()->getTypeID() != fromValue->getType()->IntegerTyID ||
            toValue->getType()->getTypeID() != toValue->getType()->IntegerTyID) {
            FATAL(type.location, "Non-integer iterator is not supported yet.");
            return nullptr;
        }
        llvm::IRBuilder<> builder(context.getCurrentBlock());
        // for(x:int in 1 to 9 by 2)... [upwards]
        // or
        // for(x:int in 9 to 1 by -2)... [downwards]
        // means
        // (x >= from && x <= to || x <= from && x => to)
        llvm::Value *xGTEFrom = builder.CreateICmpSGE(idValue, fromValue);
        llvm::Value *xLTTo = builder.CreateICmpSLE(idValue, toValue);
        llvm::Value *xLEFrom = builder.CreateICmpSLE(idValue, fromValue);
        llvm::Value *xGTTo = builder.CreateICmpSGE(idValue, toValue);
        llvm::Value *condition = builder.CreateOr(builder.CreateAnd(xGTEFrom, xLTTo),
                                                  builder.CreateAnd(xLEFrom, xGTTo));
        llvm::BranchInst::Create(loopBlock, afterBlock, condition, context.getCurrentBlock());

        context.setCurrentBlock(loopBlock);
        body.generateCode(context);
        llvm::BranchInst::Create(progressBlock, context.getCurrentBlock());
        context.setCurrentBlock(progressBlock);
        llvm::Value *increment = builder.getInt64(1);
        if (by) {
            increment = by->generateCode(context);
            if (!increment) {
                FATAL(name.location, "Invalid step given to \"for\" loop.");
                return nullptr;
            }
            if (increment->getType()->getTypeID() != fromValue->getType()->IntegerTyID) {
                FATAL(type.location, "Non-integer step in loop is not supported yet.");
                return nullptr;
            }
        }
        builder.SetInsertPoint(context.getCurrentBlock());
        idValue = (new Identifier(name.location, name.name))->generateCode(context);
        llvm::Value *newValue = builder.CreateAdd(idValue, increment);
        builder.CreateStore(newValue, Tools::getValueOfIdentifier(context, name)->second);
        llvm::BranchInst::Create(conditionBlock, context.getCurrentBlock());
        context.closeCurrentScope();
        context.setCurrentBlock(afterBlock);
        return nullptr;
    }

    llvm::Value *IfCondition::generateCode(Context &context) {
        llvm::Value *conditionValue = condition.generateCode(context);
        if (!conditionValue) {
            FATAL(location, "Invalid condition given to \"if\" statement.");
            return nullptr;
        }
        llvm::IRBuilder<> irBuilder(context.getCurrentBlock());
        conditionValue = irBuilder.CreateOr(conditionValue, irBuilder.getInt64(0));
        llvm::Type::TypeID conditionType = conditionValue->getType()->getTypeID();

        if (conditionType == conditionValue->getType()->DoubleTyID) {
            conditionValue = irBuilder.CreateFCmp(llvm::CmpInst::Predicate::FCMP_ONE, conditionValue,
                                                  llvm::ConstantFP::get(irBuilder.getDoubleTy(), 0.0));
        } else if (conditionType == conditionValue->getType()->IntegerTyID) {
            conditionValue = irBuilder.CreateICmp(llvm::CmpInst::Predicate::ICMP_NE, conditionValue,
                                                  irBuilder.getInt64(0));
        }
        llvm::Function *function = context.getCurrentBlock()->getParent();
        llvm::BasicBlock
                *then = llvm::BasicBlock::Create(context.llvmContext, "then", function),
                *otherwise = llvm::BasicBlock::Create(context.llvmContext, "otherwise", function),
                *merge = llvm::BasicBlock::Create(context.llvmContext, "merge", function);
        llvm::BranchInst::Create(then, otherwise, conditionValue, context.getCurrentBlock());
        context.createNewScope(then);
        thenBlock.generateCode(context);
        context.getCurrentBlock()->getTerminator() ||
        llvm::BranchInst::Create(merge, otherwise, conditionValue, context.getCurrentBlock());
        context.closeCurrentScope();
        context.createNewScope(otherwise);
        if (elseBlock) {
            elseBlock->generateCode(context);
        }
        context.getCurrentBlock()->getTerminator() || llvm::BranchInst::Create(merge, context.getCurrentBlock());
        context.closeCurrentScope();
        context.setCurrentBlock(merge);
        return merge;
    }

    llvm::Value *ReturnStatement::generateCode(Context &context) {
        if (context.getCurrentBlock()->getParent() == context.module->getFunction("main")) {
            FATAL(location, "Return statement outside a function.");
            return nullptr;
        }

        llvm::Type *returnType = context.getCurrentBlock()->getParent()->getReturnType();
        llvm::Value *value = expression->generateCode(context);
        if (expression) {
            if (returnType->getTypeID() == llvm::Type::VoidTyID) {
                FATAL(location, "Void function cannot return any value.");
                return nullptr;
            }
            if (!value) {
                FATAL(location, "Invalid return value.");
                return nullptr;
            }
            llvm::IRBuilder<> builder(context.getCurrentBlock());
            if (returnType->getTypeID() == llvm::Type::IntegerTyID &&
                value->getType()->getTypeID() == llvm::Type::DoubleTyID) {
                W(location, "Truncating double to fit integer return type.");
                value = builder.CreateFPToSI(value, returnType);
            } else if (returnType->getTypeID() == llvm::Type::DoubleTyID &&
                       value->getType()->getTypeID() == llvm::Type::IntegerTyID) {
                W(location, "Converting integer to fit double return type.");
                value = builder.CreateSIToFP(value, returnType);
            }
            return llvm::IRBuilder<>(context.getCurrentBlock()).CreateRet(value);
        } else return llvm::IRBuilder<>(context.getCurrentBlock()).CreateRetVoid();
    }
}
