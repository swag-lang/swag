#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "ByteCodeGen.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "Module.h"
#include "Naming.h"
#include "Report.h"
#include "Semantic.h"
#include "SemanticError.h"
#include "TypeManager.h"

bool Semantic::makeIntrinsicKindof(SemanticContext* context, AstNode* node)
{
	// Automatic convert to 'kindOf'
	// This has no sens to do a switch on an 'any'. So instead of raising an error,
	// we imply the usage of '@kindof'. That way we have a switch on the underlying type.
	const auto typeInfo = TypeManager::concretePtrRefType(node->typeInfo);
	if (typeInfo->isAny() && node->hasComputedValue())
	{
		const auto any                     = static_cast<SwagAny*>(node->computedValue->getStorageAddr());
		node->computedValue->storageOffset = node->computedValue->storageSegment->offset(reinterpret_cast<uint8_t*>(any->type));
		node->addAstFlag(AST_VALUE_IS_GEN_TYPEINFO);
		node->typeInfo = g_TypeMgr->typeInfoTypeType;
	}
	else if (typeInfo->isAny() || typeInfo->isInterface())
	{
		SWAG_CHECK(checkIsConcrete(context, node));

		node->allocateComputedValue();
		node->computedValue->storageSegment = getConstantSegFromContext(node);
		auto& typeGen                       = node->sourceFile->module->typeGen;

		TypeInfo* resultTypeInfo = nullptr;
		SWAG_CHECK(
			typeGen.genExportedTypeInfo(context, node->typeInfo, node->computedValue->storageSegment, &node->computedValue->storageOffset, GEN_EXPORTED_TYPE_SHOULD_WAIT, &
				resultTypeInfo));
		YIELD();

		node->typeInfo = resultTypeInfo;
		if (typeInfo->isAny())
			node->byteCodeFct = ByteCodeGen::emitImplicitKindOfAny;
		else
			node->byteCodeFct = ByteCodeGen::emitImplicitKindOfInterface;
	}

	return true;
}

bool Semantic::checkTypeIsNative(SemanticContext* context, TypeInfo* leftTypeInfo, TypeInfo* rightTypeInfo, AstNode* left, AstNode* right)
{
	if (leftTypeInfo->isNative() && rightTypeInfo->isNative())
		return true;
	const auto node = context->node;

	if (!leftTypeInfo->isNative())
	{
		Diagnostic diag{node->sourceFile, node->token, FMT(Err(Err0351), node->token.c_str(), leftTypeInfo->getDisplayNameC())};
		diag.addNote(left, Diagnostic::isType(leftTypeInfo));
		return context->report(diag);
	}

	Diagnostic diag{node->sourceFile, node->token, FMT(Err(Err0351), node->token.c_str(), rightTypeInfo->getDisplayNameC())};
	diag.addNote(right, Diagnostic::isType(rightTypeInfo));
	return context->report(diag);
}

bool Semantic::checkTypeIsNative(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
	SWAG_VERIFY(typeInfo->isNative(), SemanticError::notAllowedError(context, node, typeInfo));
	return true;
}

bool Semantic::sendCompilerMsgTypeDecl(SemanticContext* context)
{
	const auto sourceFile = context->sourceFile;
	const auto module     = sourceFile->module;
	const auto node       = context->node;

	// Filter what we send
	if (module->kind == ModuleKind::BootStrap || module->kind == ModuleKind::Runtime)
		return true;
	if (sourceFile->imported)
		return true;
	if (!node->ownerScope->isGlobalOrImpl())
		return true;
	if (node->hasAstFlag(AST_IS_GENERIC | AST_FROM_GENERIC))
		return true;
	if (node->typeInfo->isTuple() || node->typeInfo->isGeneric())
		return true;

	CompilerMessage msg;
	msg.concrete.kind        = CompilerMsgKind::SemTypes;
	msg.concrete.name.buffer = node->token.text.buffer;
	msg.concrete.name.count  = node->token.text.length();
	msg.typeInfo             = node->typeInfo;
	msg.node                 = node;
	SWAG_CHECK(module->postCompilerMessage(context, msg));

	if (node->hasAttribute(ATTRIBUTE_GEN))
	{
		msg.concrete.kind = CompilerMsgKind::AttributeGen;
		SWAG_CHECK(module->postCompilerMessage(context, msg));
	}

	return true;
}

bool Semantic::checkIsConcrete(SemanticContext* context, AstNode* node)
{
	if (!node)
		return true;
	if (node->hasAstFlag(AST_R_VALUE))
		return true;
	if (node->hasComputedValue() && !node->hasAstFlag(AST_NO_BYTECODE))
		return true;

	if (node->kind == AstNodeKind::TypeExpression || node->kind == AstNodeKind::TypeLambda)
		return context->report({node, Err(Err0284)});
	if (node->hasAstFlag(AST_FROM_GENERIC_REPLACE))
		return context->report({node, Err(Err0284)});

	if (!node->resolvedSymbolName)
		return true;

	// If this is an identifier ref, then we need to be check concrete from left to right,
	// to raise an error on the first problem, and not only the result
	if (node->kind == AstNodeKind::IdentifierRef)
	{
		for (const auto c : node->children)
		{
			if (c == node)
				break;
			SWAG_CHECK(checkIsConcrete(context, c));
		}
	}

	// Reference to a static struct member
	if (node->resolvedSymbolOverload && node->resolvedSymbolOverload->hasFlag(OVERLOAD_VAR_STRUCT))
	{
		const Diagnostic  diag{node, FMT(Err(Err0590), node->resolvedSymbolOverload->symbol->ownerTable->scope->name.c_str())};
		const Diagnostic* note = nullptr;

		// Missing self ?
		if (node->children.size() <= 1 &&
			node->ownerStructScope &&
			node->ownerStructScope == context->node->ownerStructScope &&
			node->ownerFct)
		{
			if (node->ownerStructScope->symTable.find(node->resolvedSymbolName->name))
			{
				const auto nodeFct = castAst<AstFuncDecl>(node->ownerFct, AstNodeKind::FuncDecl);
				const auto typeFct = castTypeInfo<TypeInfoFuncAttr>(node->ownerFct->typeInfo, TypeInfoKind::FuncAttr);
				if (typeFct->parameters.empty() || !typeFct->parameters[0]->typeInfo->isSelf())
					note = Diagnostic::note(node->ownerFct, node->ownerFct->token, Nte(Nte0041));
				else if (!typeFct->parameters.empty() && typeFct->parameters[0]->typeInfo->isSelf() && !typeFct->parameters[0]->typeInfo->hasFlag(TYPEINFO_HAS_USING))
					note = Diagnostic::note(nodeFct->parameters->children.front(), Nte(Nte0027));
				else
					note = Diagnostic::note(Nte(Nte0045));
			}
		}

		return context->report(diag, note);
	}

	const Diagnostic  diag{node, FMT(Err(Err0589), Naming::kindName(node->resolvedSymbolName->kind).c_str(), node->resolvedSymbolName->name.c_str())};
	const Diagnostic* note = nullptr;

	// struct.field
	if (node->resolvedSymbolName && node->resolvedSymbolName->kind == SymbolKind::Struct)
		note = Diagnostic::note(FMT(Nte(Nte0088), node->resolvedSymbolName->name.c_str(), node->resolvedSymbolName->name.c_str()));

	return context->report(diag, note);
}

bool Semantic::checkIsConcreteOrType(SemanticContext* context, AstNode* node, bool typeOnly)
{
	if (node->hasAstFlag(AST_R_VALUE))
		return true;

	if (node->kind == AstNodeKind::TypeExpression ||
		node->kind == AstNodeKind::TypeLambda ||
		(node->kind == AstNodeKind::IdentifierRef && node->hasAstFlag(AST_FROM_GENERIC_REPLACE)) ||
		(node->resolvedSymbolName && node->resolvedSymbolName->kind == SymbolKind::Struct) ||
		(node->resolvedSymbolName && node->resolvedSymbolName->kind == SymbolKind::TypeAlias) ||
		(node->resolvedSymbolName && node->resolvedSymbolName->kind == SymbolKind::Interface) ||
		(node->resolvedSymbolName && node->resolvedSymbolName->kind == SymbolKind::Attribute) ||
		(node->resolvedSymbolName && node->resolvedSymbolName->kind == SymbolKind::Namespace) ||
		(node->resolvedSymbolName && node->resolvedSymbolName->kind == SymbolKind::Enum))
	{
		TypeInfo* result = nullptr;
		SWAG_CHECK(resolveTypeAsExpression(context, node, &result));
		YIELD();
		node->typeInfo = result;
		node->removeAstFlag(AST_NO_BYTECODE);
		return true;
	}

	if (typeOnly)
		return true;
	return checkIsConcrete(context, node);
}

bool Semantic::resolveTypeLambdaClosure(SemanticContext* context)
{
	const auto node     = castAst<AstTypeLambda>(context->node, AstNodeKind::TypeLambda, AstNodeKind::TypeClosure);
	const auto typeInfo = makeType<TypeInfoFuncAttr>(TypeInfoKind::LambdaClosure);
	typeInfo->declNode  = node;

	if (node->hasSpecFlag(AstTypeLambda::SPECFLAG_CAN_THROW))
		typeInfo->addFlag(TYPEINFO_CAN_THROW);

	if (node->returnType)
	{
		typeInfo->returnType = node->returnType->typeInfo;
		if (typeInfo->returnType->isGeneric())
			typeInfo->addFlag(TYPEINFO_GENERIC);
	}
	else
	{
		typeInfo->returnType = g_TypeMgr->typeInfoVoid;
	}

	if (node->parameters)
	{
		size_t index = 0;
		for (auto param : node->parameters->children)
		{
			auto typeParam      = TypeManager::makeParam();
			typeParam->typeInfo = param->typeInfo;
			typeParam->declNode = param;

			if (param->hasExtMisc() && param->extMisc()->isNamed)
				typeParam->name = param->extMisc()->isNamed->token.text;

			if (typeParam->typeInfo->isGeneric())
				typeInfo->addFlag(TYPEINFO_GENERIC);

			// Variadic must be the last one
			if (typeParam->typeInfo->isVariadic())
			{
				typeInfo->addFlag(TYPEINFO_VARIADIC);
				if (index != node->parameters->children.size() - 1)
					return context->report({param, Err(Err0513)});
			}
			else if (typeParam->typeInfo->isTypedVariadic())
			{
				typeInfo->addFlag(TYPEINFO_TYPED_VARIADIC);
				if (index != node->parameters->children.size() - 1)
					return context->report({param, Err(Err0513)});
			}
			else if (typeParam->typeInfo->isCVariadic())
			{
				typeInfo->addFlag(TYPEINFO_C_VARIADIC);
				if (index != node->parameters->children.size() - 1)
					return context->report({param, Err(Err0513)});
			}

			typeInfo->parameters.push_back(typeParam);
			index++;
		}

		if (node->parameters->hasAstFlag(AST_IN_TYPE_VAR_DECLARATION))
			SWAG_CHECK(setupFuncDeclParams(context, typeInfo, node, node->parameters, false));
	}

	typeInfo->computeName();

	if (node->kind == AstNodeKind::TypeClosure)
	{
		typeInfo->sizeOf = SWAG_LIMIT_CLOSURE_SIZEOF;
		typeInfo->addFlag(TYPEINFO_CLOSURE);
	}
	else
	{
		typeInfo->sizeOf = sizeof(void*);
	}

	node->typeInfo = typeInfo;

	return true;
}

void Semantic::forceConstType(SemanticContext* context, AstTypeExpression* node)
{
	const auto concrete = node->typeInfo->getConcreteAlias();
	if (concrete->isStruct() ||
		concrete->isArray() ||
		concrete->isClosure() ||
		concrete->isPointer() ||
		concrete->isSlice())
	{
		if (node->typeFlags & TYPEFLAG_FORCE_CONST)
			node->typeFlags |= TYPEFLAG_IS_CONST;
		if (node->typeFlags & TYPEFLAG_IS_CONST)
			node->typeInfo = g_TypeMgr->makeConst(node->typeInfo);
	}
}

bool Semantic::resolveType(SemanticContext* context)
{
	auto typeNode = castAst<AstTypeExpression>(context->node, AstNodeKind::TypeExpression);

	// Array with predefined dimensions, we evaluate all dimensions as const
	// Do it first because of potential pending
	if (typeNode->arrayDim && typeNode->arrayDim != UINT8_MAX)
	{
		for (int i = typeNode->arrayDim - 1; i >= 0; i--)
		{
			const auto child = typeNode->children[i];
			SWAG_CHECK(evaluateConstExpression(context, child));
			YIELD();
			if (child->hasComputedValue())
				child->addAstFlag(AST_NO_BYTECODE);
		}
	}

	// Already solved
	if (typeNode->hasAstFlag(AST_FROM_GENERIC) &&
		typeNode->typeInfo &&
		!typeNode->typeInfo->isUndefined())
	{
		// Count is generic, need to reevaluate
		if (typeNode->typeInfo->hasFlag(TYPEINFO_GENERIC_COUNT))
		{
			SWAG_ASSERT(typeNode->typeInfo->isArray());
			typeNode->typeInfo = nullptr;
		}
		else
		{
			forceConstType(context, typeNode);
			return true;
		}
	}

	if ((typeNode->hasSemFlag(SEMFLAG_TYPE_SOLVED)) &&
		typeNode->typeInfo &&
		!typeNode->typeInfo->isUndefined())
	{
		forceConstType(context, typeNode);
		return true;
	}

	// Code
	if (typeNode->typeFlags & TYPEFLAG_IS_CODE)
	{
		const auto typeP = typeNode->findParent(AstNodeKind::FuncDeclParam);
		SWAG_VERIFY(typeP && typeNode->ownerFct, context->report({typeNode, FMT(Err(Err0512), "code")}));
		typeNode->typeInfo = g_TypeMgr->typeInfoCode;
		return true;
	}

	// cvarargs
	if (typeNode->typeFromLiteral && typeNode->typeFromLiteral->hasFlag(TYPEINFO_C_VARIADIC))
	{
		const auto typeP = typeNode->findParent(AstNodeKind::FuncDeclParam);
		SWAG_VERIFY(typeP && typeNode->ownerFct, context->report({typeNode, FMT(Err(Err0512), "cvarargs")}));
		typeNode->typeInfo = g_TypeMgr->typeInfoCVariadic;
		return true;
	}

	if (typeNode->identifier)
	{
		typeNode->typeInfo = typeNode->identifier->typeInfo;
		typeNode->inheritAstFlagsOr(typeNode->identifier, AST_IS_GENERIC);
	}
	else if (typeNode->typeFlags & TYPEFLAG_IS_SUB_TYPE)
	{
		typeNode->typeInfo = typeNode->children.back()->typeInfo;
	}
	else
	{
		if (!typeNode->typeFromLiteral)
			typeNode->typeFromLiteral = TypeManager::literalTypeToType(typeNode->literalType);
		typeNode->typeInfo = typeNode->typeFromLiteral;

		// Typed variadic ?
		if (typeNode->typeInfo->isVariadic() && !typeNode->children.empty())
		{
			const auto typeVariadic = castTypeInfo<TypeInfoVariadic>(typeNode->typeInfo->clone());
			typeVariadic->kind      = TypeInfoKind::TypedVariadic;
			typeVariadic->rawType   = typeNode->children.front()->typeInfo;
			typeVariadic->addFlag(typeVariadic->rawType->flags & TYPEINFO_GENERIC);
			typeVariadic->forceComputeName();
			typeNode->typeFromLiteral = typeVariadic;
			typeNode->typeInfo        = typeVariadic;
		}
	}

	// This is a generic type, not yet known
	if (!typeNode->typeInfo && typeNode->identifier && typeNode->identifier->resolvedSymbolOverload->hasFlag(OVERLOAD_GENERIC))
	{
		typeNode->resolvedSymbolName     = typeNode->identifier->resolvedSymbolName;
		typeNode->resolvedSymbolOverload = typeNode->identifier->resolvedSymbolOverload;
		typeNode->typeInfo               = makeType<TypeInfoGeneric>();
		typeNode->typeInfo->name         = typeNode->resolvedSymbolName->name;
		typeNode->typeInfo               = typeNode->typeInfo;
	}

	// Otherwise, this is strange, we should have a type
	SWAG_VERIFY(typeNode->typeInfo, Report::internalError(context->node, "resolveType, null type !"));

	// If type comes from an identifier, be sure it's a type
	if (typeNode->identifier && !typeNode->identifier->children.empty())
	{
		const auto child = typeNode->identifier->children.back();
		if (!child->typeInfo || !child->typeInfo->isUndefined())
		{
			if (child->resolvedSymbolName)
			{
				const auto symName = child->resolvedSymbolName;
				const auto symOver = child->resolvedSymbolOverload;
				if (symName->kind != SymbolKind::Enum &&
					symName->kind != SymbolKind::TypeAlias &&
					symName->kind != SymbolKind::GenericType &&
					symName->kind != SymbolKind::Struct &&
					symName->kind != SymbolKind::Interface)
				{
					Diagnostic        diag{child->sourceFile, child->token, FMT(Err(Err0399), child->token.c_str(), Naming::aKindName(symName->kind).c_str())};
					const Diagnostic* note = Diagnostic::hereIs(symOver);
					if ((typeNode->typeFlags & TYPEFLAG_IS_PTR) && symName->kind == SymbolKind::Variable)
					{
						if (symOver->typeInfo->isPointer())
						{
							diag.hint        = Nte(Nte0160);
							const auto note1 = Diagnostic::note(FMT(Nte(Nte0182), symName->name.c_str(), symName->name.c_str()));
							return context->report(diag, note1, note);
						}

						diag.hint = Nte(Nte0160);
						return context->report(diag, note);
					}

					return context->report(diag, note);
				}
			}
		}
	}

	// In fact, this is a pointer
	if (typeNode->typeFlags & TYPEFLAG_IS_PTR)
	{
		auto ptrFlags = typeNode->typeInfo->flags & TYPEINFO_GENERIC;
		if (typeNode->typeFlags & TYPEFLAG_IS_SELF)
			ptrFlags |= TYPEINFO_SELF;
		if (typeNode->typeFlags & TYPEFLAG_HAS_USING)
			ptrFlags |= TYPEINFO_HAS_USING;
		if (typeNode->typeFlags & TYPEFLAG_IS_CONST)
			ptrFlags |= TYPEINFO_CONST;
		if (typeNode->typeFlags & TYPEFLAG_IS_PTR_ARITHMETIC)
			ptrFlags |= TYPEINFO_POINTER_ARITHMETIC;
		if (ptrFlags & TYPEINFO_GENERIC)
			typeNode->addAstFlag(AST_IS_GENERIC);
		typeNode->typeInfo = g_TypeMgr->makePointerTo(typeNode->typeInfo, ptrFlags);
		return true;
	}

	if (!(typeNode->typeFlags & TYPEFLAG_IS_REF))
		forceConstType(context, typeNode);

	// In fact, this is a slice
	if (typeNode->typeFlags & TYPEFLAG_IS_SLICE)
	{
		const auto ptrSlice   = makeType<TypeInfoSlice>();
		ptrSlice->pointedType = typeNode->typeInfo;
		if (typeNode->typeFlags & TYPEFLAG_IS_CONST)
			ptrSlice->flags |= TYPEINFO_CONST;
		ptrSlice->flags |= ptrSlice->pointedType->flags & TYPEINFO_GENERIC;
		typeNode->typeInfo = ptrSlice;
		ptrSlice->computeName();
		return true;
	}

	// In fact this is a reference
	if (typeNode->typeFlags & TYPEFLAG_IS_REF)
	{
		auto ptrFlags = TYPEINFO_POINTER_REF;
		if (typeNode->typeFlags & TYPEFLAG_IS_CONST)
			ptrFlags |= TYPEINFO_CONST;
		ptrFlags |= typeNode->typeInfo->flags & TYPEINFO_GENERIC;

		if (typeNode->typeFlags & TYPEFLAG_IS_MOVE_REF)
		{
			const auto typeP = typeNode->findParent(AstNodeKind::FuncDeclParam);
			SWAG_VERIFY(typeP && typeNode->ownerFct, context->report({typeNode, FMT(Err(Err0512), "&&")}));
			ptrFlags |= TYPEINFO_POINTER_MOVE_REF;
		}

		const auto typeRef = g_TypeMgr->makePointerTo(typeNode->typeInfo, ptrFlags);
		typeNode->typeInfo = typeRef;
		return true;
	}

	// In fact, this is an array
	if (typeNode->typeFlags & TYPEFLAG_IS_ARRAY)
	{
		SWAG_ASSERT(typeNode->arrayDim);

		// Array without a specified size
		if (typeNode->arrayDim == UINT8_MAX)
		{
			const auto ptrArray   = makeType<TypeInfoArray>();
			ptrArray->count       = UINT32_MAX;
			ptrArray->totalCount  = UINT32_MAX;
			ptrArray->pointedType = typeNode->typeInfo;
			ptrArray->finalType   = typeNode->typeInfo;
			if (typeNode->typeFlags & TYPEFLAG_IS_CONST)
				ptrArray->flags |= TYPEINFO_CONST;
			ptrArray->flags |= ptrArray->finalType->flags & TYPEINFO_GENERIC;
			ptrArray->sizeOf = 0;
			ptrArray->computeName();
			typeNode->typeInfo = ptrArray;
			return true;
		}

		const auto rawType = typeNode->typeInfo;
		SWAG_VERIFY(!rawType->isVoid(), context->report({typeNode->children.back(), Err(Err0736)}));

		uint32_t totalCount = 1;
		for (int i = typeNode->arrayDim - 1; i >= 0; i--)
		{
			auto child = typeNode->children[i];

			uint32_t count        = 0;
			bool     genericCount = false;

			if (child->kind == AstNodeKind::IdentifierRef &&
				!child->hasAstFlag(AST_CONST_EXPR) &&
				typeNode->ownerStructScope &&
				typeNode->ownerStructScope->owner->typeInfo->isGeneric())
			{
				genericCount = true;
			}
			else if (!child->hasComputedValue() &&
				child->resolvedSymbolOverload &&
				(child->resolvedSymbolOverload->hasFlag(OVERLOAD_GENERIC)))
			{
				genericCount = true;
			}
			else
			{
				SWAG_CHECK(checkIsConstExpr(context, child->hasComputedValue(), child, Err(Err0036)));
				count = child->computedValue->reg.u32;
			}

			const auto childType = TypeManager::concreteType(child->typeInfo);
			SWAG_VERIFY(childType->isNativeInteger(), context->report({child, FMT(Err(Err0211), child->typeInfo->getDisplayNameC())}));
			SWAG_CHECK(context->checkSizeOverflow("array", count * rawType->sizeOf, SWAG_LIMIT_ARRAY_SIZE));
			SWAG_VERIFY(!child->isConstant0(), context->report({child, Err(Err0210)}));

			const auto ptrArray = makeType<TypeInfoArray>();
			ptrArray->count     = count;
			totalCount *= ptrArray->count;
			SWAG_CHECK(context->checkSizeOverflow("array", totalCount * rawType->sizeOf, SWAG_LIMIT_ARRAY_SIZE));
			ptrArray->totalCount  = totalCount;
			ptrArray->pointedType = typeNode->typeInfo;
			ptrArray->finalType   = rawType;
			ptrArray->sizeOf      = ptrArray->count * ptrArray->pointedType->sizeOf;
			if (typeNode->typeFlags & TYPEFLAG_IS_CONST)
				ptrArray->flags |= TYPEINFO_CONST;
			ptrArray->flags |= ptrArray->finalType->flags & TYPEINFO_GENERIC;

			if (genericCount)
			{
				ptrArray->flags |= TYPEINFO_GENERIC | TYPEINFO_GENERIC_COUNT;
				ptrArray->sizeNode = child;
			}

			ptrArray->computeName();
			typeNode->typeInfo = ptrArray;
		}

		return true;
	}

	// Be sure we do not have a useless user 'const'
	const auto typeC = TypeManager::concreteType(typeNode->typeInfo);
	if (typeNode->typeFlags & TYPEFLAG_HAS_LOC_CONST &&
		!typeC->isPointer() &&
		!typeC->isArray() &&
		!typeC->isStruct())
	{
		const Diagnostic diag{typeNode->sourceFile, typeNode->locConst, FMT(Err(Err0395), typeNode->typeInfo->getDisplayNameC())};
		return context->report(diag);
	}

	return true;
}

bool Semantic::resolveTypeAliasBefore(SemanticContext* context)
{
	const auto node = context->node;

	// Collect all attributes for the variable
	SWAG_CHECK(collectAttributes(context, node, nullptr));

	const auto typeInfo = makeType<TypeInfoAlias>();
	typeInfo->declNode  = node;
	typeInfo->name      = node->token.text;
	if (node->hasAttribute(ATTRIBUTE_STRICT))
		typeInfo->addFlag(TYPEINFO_STRICT);
	node->typeInfo = typeInfo;

	uint32_t symbolFlags = OVERLOAD_INCOMPLETE;
	if (node->typeInfo->isGeneric())
		symbolFlags |= OVERLOAD_GENERIC;

	AddSymbolTypeInfo toAdd;
	toAdd.node     = node;
	toAdd.typeInfo = node->typeInfo;
	toAdd.kind     = SymbolKind::TypeAlias;
	toAdd.flags    = symbolFlags;

	node->resolvedSymbolOverload = node->ownerScope->symTable.addSymbolTypeInfo(context, toAdd);
	SWAG_CHECK(node->resolvedSymbolOverload);
	if (node->hasAttribute(ATTRIBUTE_PUBLIC))
		node->ownerScope->addPublicNode(node);
	return true;
}

bool Semantic::resolveTypeAlias(SemanticContext* context)
{
	const auto node     = context->node;
	const auto typeInfo = castTypeInfo<TypeInfoAlias>(node->typeInfo, TypeInfoKind::Alias);
	typeInfo->rawType   = node->children.front()->typeInfo;
	typeInfo->sizeOf    = typeInfo->rawType->sizeOf;
	typeInfo->addFlag(typeInfo->rawType->flags & TYPEINFO_GENERIC);
	typeInfo->addFlag(typeInfo->rawType->flags & TYPEINFO_CONST);
	typeInfo->computeName();
	uint32_t symbolFlags = node->resolvedSymbolOverload->flags & ~OVERLOAD_INCOMPLETE;
	if (typeInfo->isGeneric())
		symbolFlags |= OVERLOAD_GENERIC;

	AddSymbolTypeInfo toAdd;
	toAdd.node     = node;
	toAdd.typeInfo = node->typeInfo;
	toAdd.kind     = SymbolKind::TypeAlias;
	toAdd.flags    = symbolFlags;
	SWAG_CHECK(node->ownerScope->symTable.addSymbolTypeInfo(context, toAdd));
	return true;
}

bool Semantic::resolveExplicitBitCast(SemanticContext* context)
{
	const auto node     = castAst<AstCast>(context->node, AstNodeKind::Cast);
	const auto typeNode = node->children[0];
	const auto exprNode = node->children[1];

	SWAG_CHECK(checkIsConcrete(context, exprNode));

	const auto typeInfo     = TypeManager::concreteType(typeNode->typeInfo);
	const auto exprTypeInfo = TypeManager::concreteType(exprNode->typeInfo);

	if (!typeInfo->isNativeInteger() &&
		!typeInfo->isNativeFloat() &&
		!typeInfo->isRune())
	{
		const Diagnostic diag{typeNode, FMT(Err(Err0230), typeInfo->getDisplayNameC())};
		return context->report(diag);
	}

	if (!exprTypeInfo->isNativeInteger() &&
		!exprTypeInfo->isNativeFloat() &&
		!exprTypeInfo->isRune() &&
		!exprTypeInfo->isPointer())
	{
		const Diagnostic diag{exprNode, FMT(Err(Err0228), exprTypeInfo->getDisplayNameC())};
		return context->report(diag);
	}

	if (typeInfo->sizeOf > exprTypeInfo->sizeOf)
	{
		const Diagnostic diag{exprNode, FMT(Err(Err0229), exprTypeInfo->getDisplayNameC(), typeInfo->getDisplayNameC())};
		return context->report(diag);
	}

	node->typeInfo    = typeNode->typeInfo;
	node->byteCodeFct = ByteCodeGen::emitPassThrough;
	node->inheritAstFlagsOr(exprNode, AST_CONST_EXPR | AST_VALUE_COMPUTED | AST_R_VALUE | AST_L_VALUE | AST_SIDE_EFFECTS);
	node->inheritComputedValue(exprNode);
	node->resolvedSymbolName     = exprNode->resolvedSymbolName;
	node->resolvedSymbolOverload = exprNode->resolvedSymbolOverload;

	if (node->hasComputedValue() && node->typeInfo->isNative())
	{
		switch (node->typeInfo->nativeType)
		{
		case NativeTypeKind::S8:
			node->computedValue->reg.s64 = node->computedValue->reg.s8;
			break;
		case NativeTypeKind::S16:
			node->computedValue->reg.s64 = node->computedValue->reg.s16;
			break;
		case NativeTypeKind::S32:
			node->computedValue->reg.s64 = node->computedValue->reg.s32;
			break;
		case NativeTypeKind::U8:
			node->computedValue->reg.u64 = node->computedValue->reg.u8;
			break;
		case NativeTypeKind::U16:
			node->computedValue->reg.u64 = node->computedValue->reg.u16;
			break;
		case NativeTypeKind::U32:
			node->computedValue->reg.u64 = node->computedValue->reg.u32;
			break;
		default:
			break;
		}
	}

	return true;
}

bool Semantic::resolveExplicitCast(SemanticContext* context)
{
	const auto node     = castAst<AstCast>(context->node, AstNodeKind::Cast);
	const auto typeNode = node->children[0];
	const auto exprNode = node->children[1];

	SWAG_CHECK(checkIsConcrete(context, exprNode));

	// When we cast from a structure to an interface, we need to be sure that every interfaces are
	// registered in the structure type, otherwise the cast can fail depending on the compile order
	exprNode->typeInfo      = getConcreteTypeUnRef(exprNode, CONCRETE_FUNC | CONCRETE_ALIAS);
	const auto exprTypeInfo = exprNode->typeInfo;
	if (typeNode->typeInfo->isInterface() && exprTypeInfo->isStruct())
	{
		waitAllStructInterfaces(context->baseJob, exprTypeInfo);
		YIELD();
	}

	uint32_t castFlags = CASTFLAG_EXPLICIT | CASTFLAG_ACCEPT_PENDING;
	if (node->hasSpecFlag(AstCast::SPECFLAG_UN_CONST))
		castFlags |= CASTFLAG_FORCE_UN_CONST;
	if (node->hasSpecFlag(AstCast::SPECFLAG_OVERFLOW))
		castFlags |= CASTFLAG_CAN_OVERFLOW;
	SWAG_CHECK(TypeManager::makeCompatibles(context, typeNode->typeInfo, nullptr, exprNode, castFlags));
	YIELD();

	node->typeInfo       = typeNode->typeInfo;
	node->toCastTypeInfo = typeNode->typeInfo;

	node->byteCodeFct = ByteCodeGen::emitExplicitCast;
	node->inheritAstFlagsOr(exprNode, AST_CONST_EXPR | AST_VALUE_IS_GEN_TYPEINFO | AST_VALUE_COMPUTED | AST_R_VALUE | AST_L_VALUE | AST_SIDE_EFFECTS | AST_OP_AFFECT_CAST);
	node->inheritComputedValue(exprNode);
	node->resolvedSymbolName     = exprNode->resolvedSymbolName;
	node->resolvedSymbolOverload = exprNode->resolvedSymbolOverload;

	// In case case has triggered a special function call, need to get it
	// (usage of opAffect)
	if (exprNode->hasExtMisc() && exprNode->extMisc()->resolvedUserOpSymbolOverload)
	{
		node->allocateExtension(ExtensionKind::Misc);
		node->extMisc()->resolvedUserOpSymbolOverload = exprNode->extMisc()->resolvedUserOpSymbolOverload;
	}

	// Revert the implicit cast information
	// Requested type will be stored in typeInfo of node, and previous type will be stored in typeInfo of exprNode
	// (we cannot use castedTypeInfo from node, because an explicit cast result could be casted itself with an implicit cast)
	if (exprNode->castedTypeInfo)
	{
		if (!node->hasAstFlag(AST_VALUE_COMPUTED | AST_OP_AFFECT_CAST))
		{
			exprNode->typeInfo       = exprNode->castedTypeInfo;
			exprNode->castedTypeInfo = nullptr;
		}

		// In case of a computed value, we need to remember the type we come from to make
		// last minutes cast in emitLiteral
		else
			node->castedTypeInfo = exprNode->castedTypeInfo;
	}

	return true;
}

bool Semantic::resolveExplicitAutoCast(SemanticContext* context)
{
	const auto node     = castAst<AstCast>(context->node, AstNodeKind::AutoCast);
	const auto exprNode = node->children[0];

	exprNode->typeInfo   = getConcreteTypeUnRef(exprNode, CONCRETE_FUNC | CONCRETE_ALIAS);
	const auto cloneType = exprNode->typeInfo->clone();
	cloneType->flags |= TYPEINFO_AUTO_CAST;
	node->typeInfo = cloneType;

	node->byteCodeFct = ByteCodeGen::emitExplicitAutoCast;
	node->inheritAstFlagsOr(exprNode, AST_CONST_EXPR | AST_VALUE_IS_GEN_TYPEINFO | AST_VALUE_COMPUTED | AST_SIDE_EFFECTS);
	node->inheritComputedValue(exprNode);
	return true;
}

bool Semantic::resolveTypeList(SemanticContext* context)
{
	const auto node = context->node;
	if (node->children.size() == 1)
		node->typeInfo = node->children.front()->typeInfo;
	return true;
}

bool Semantic::resolveTypeAsExpression(SemanticContext* context, AstNode* node, TypeInfo** resultTypeInfo, uint32_t flags)
{
	SWAG_CHECK(resolveTypeAsExpression(context, node, node->typeInfo, resultTypeInfo, flags));
	return true;
}

bool Semantic::resolveTypeAsExpression(SemanticContext* context, AstNode* node, TypeInfo* typeInfo, TypeInfo** resultTypeInfo, uint32_t flags)
{
	const auto sourceFile = context->sourceFile;
	const auto module     = sourceFile->module;
	auto&      typeGen    = module->typeGen;

	node->allocateComputedValue();
	node->computedValue->reg.pointer    = reinterpret_cast<uint8_t*>(typeInfo);
	node->computedValue->storageSegment = getConstantSegFromContext(node);
	SWAG_CHECK(typeGen.genExportedTypeInfo(context, typeInfo, node->computedValue->storageSegment, &node->computedValue->storageOffset, GEN_EXPORTED_TYPE_SHOULD_WAIT | flags, resultTypeInfo));
	YIELD();
	node->setFlagsValueIsComputed();
	node->addAstFlag(AST_VALUE_IS_GEN_TYPEINFO);
	return true;
}
