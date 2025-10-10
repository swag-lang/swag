#include "pch.h"
#include "Backend/ByteCode/Sanity/ByteCodeSanity.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/ByteCode_Math.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Backend/ByteCode/Sanity/ByteCodeSanity_Macros.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/Naming.h"

bool ByteCodeSanity::mustEmitSafety(SafetyFlags what) const
{
    const auto ip   = STATE()->ip;
    const auto node = ip->node;
    if (!node || !context.sourceFile || !context.sourceFile->module || !context.sourceFile->module->mustEmitSafety(node, what, true))
        return false;
    return true;
}

Diagnostic* ByteCodeSanity::raiseError(const ByteCodeInstruction* ip, const Utf8& msg, const SanityValue* locValue, AstNode* locNode)
{
    if (!ip->node)
        return nullptr;
    if (!locNode)
        locNode = ip->node;
    const auto err = new Diagnostic{locNode, locNode->token, msg};
    return raiseError(ip, err, locValue, locNode);
}

Diagnostic* ByteCodeSanity::raiseError(const ByteCodeInstruction* ip, Diagnostic* err, const SanityValue* locValue, AstNode* locNode)
{
    if (!ip->node)
        return nullptr;
    if (!locNode)
        locNode = ip->node;
    if (!locValue)
        return err;

    VectorNative<SymbolOverload*>      doneOverload;
    VectorNative<AstNode*>             doneNodes;
    VectorNative<ByteCodeInstruction*> ips;
    VectorNative<AstNode*>             ipNodes;

    for (const auto it : locValue->ips)
    {
        ips.push_back(it);
        auto ipNode = it->node;
        while (ipNode->hasOwnerInline())
            ipNode = ipNode->ownerInline();
        ipNodes.push_back(ipNode);
    }

    if (locNode)
    {
        ips.push_back(nullptr);
        ipNodes.push_back(locNode);
        doneNodes.push_back(locNode);
    }

    const auto isHere = [&](const AstNode* node) {
        for (const auto it : doneNodes)
        {
            if (it->token.startLocation == node->token.startLocation && it->token.endLocation == node->token.endLocation)
                return true;
        }
        return false;
    };

    for (uint32_t i = ipNodes.size() - 1; i != UINT32_MAX; i--)
    {
        const auto ipNode = ipNodes[i];
        if (!ipNode)
            continue;

        const auto overload = ipNode->resolvedSymbolOverload();

        if (!isHere(ipNode) && (!overload || ipNode != overload->node))
        {
            err->setForceFromContext(true);
            if (ipNode->hasComputedValue() && ipNode->typeInfo->isPointerNull())
                err->addNote(ipNode, ipNode->token, "from null value");
            else if (ipNode->hasComputedValue())
                err->addNote(ipNode, ipNode->token, "from compile-time value");
            else if (ipNode->is(AstNodeKind::Return) && ipNode->hasOwnerInline())
                err->addNote(ipNode->ownerInline(), form("from return value of inlined function [[%s]]", ipNode->ownerInline()->func->tokenName.cstr()));
            else
                err->addNote(ipNode, ipNode->token, "from");
            err->setForceFromContext(false);
            doneNodes.push_back(ipNode);
        }

        // Show the symbol declaration the last time it's present in an ip node
        if (overload)
        {
            if (!doneOverload.contains(overload) && !isHere(overload->node))
            {
                bool here = false;
                for (uint32_t j = 0; j < i; j++)
                {
                    if (ipNodes[j] && ipNodes[j]->resolvedSymbolOverload() == overload)
                    {
                        here = true;
                        break;
                    }
                }

                if (!here)
                {
                    err->setForceFromContext(true);
                    doneOverload.push_back(overload);

                    Utf8 what = form("declaration of %s [[%s]]", Naming::kindName(overload).cstr(), overload->symbol->name.cstr());
                    err->addNote(overload->node, overload->node->getTokenName(), what);

                    doneNodes.push_back(overload->node);
                    err->setForceFromContext(false);
                }
            }
        }
    }

    return err;
}

bool ByteCodeSanity::checkOverflow(bool isValid, SafetyMsg msgKind, const TypeInfo* type, const void* val0, const void* val1)
{
    if (isValid)
        return true;
    return context.overflowError(STATE()->ip->node, msgKind, type, val0, val1);
}

bool ByteCodeSanity::checkDivZero(const SanityValue* value, bool isZero)
{
    if (!value->isConstant() || !isZero)
        return true;
    if (mustEmitSafety(SAFETY_MATH))
        return true;

    const auto ip  = STATE()->ip;
    const auto err = raiseError(ip, toErr(Saf0016), value);
    if (err)
        return context.report(*err);
    return true;
}

bool ByteCodeSanity::checkEscapeFrame(const SanityValue* value)
{
    SWAG_ASSERT(value->reg.u32 < UINT32_MAX);
    if (mustEmitSafety(SAFETY_MEMORY))
        return true;

    const auto err = raiseError(STATE()->ip, toErr(Saf0032), value);
    if (err)
        return context.report(*err);
    return true;
}

bool ByteCodeSanity::checkNotNull(const SanityValue* value)
{
    if (!value->isZero())
        return true;
    if (mustEmitSafety(SAFETY_NULL))
        return true;

    const auto err = raiseError(STATE()->ip, toErr(Saf0018), value);
    if (err)
        return context.report(*err);
    return true;
}

bool ByteCodeSanity::checkNotNullReturn(uint32_t reg)
{
    const auto ip = STATE()->ip;
    if (ip->flags.has(BCI_NOT_NULL))
        return true;
    if (mustEmitSafety(SAFETY_NULL))
        return true;

    SanityValue* ra = nullptr;
    SWAG_CHECK(STATE()->getRegister(ra, reg));

    if (ra->isZero())
    {
        if (!context.bc->typeInfoFunc->returnType->isNullable() && context.bc->typeInfoFunc->returnType->couldBeNull())
        {
            const auto returnType = context.bc->typeInfoFunc->concreteReturnType();
            if (!returnType->isNullable() && returnType->couldBeNull())
            {
                const auto err = raiseError(STATE()->ip, toErr(Saf0031), ra);
                if (err)
                    return context.report(*err);
            }
        }
    }

    return true;
}

bool ByteCodeSanity::checkNotNullArguments(VectorNative<uint32_t> pushParams, const Utf8& intrinsic)
{
    if (!mustEmitSafety(SAFETY_NULL))
        return true;

    const auto        ip       = STATE()->ip;
    TypeInfoFuncAttr* typeFunc = nullptr;

    if (intrinsic.empty())
    {
        if (!ip->node || !ip->node->is(AstNodeKind::Identifier))
            return true;
        const auto ident = castAst<AstIdentifier>(ip->node, AstNodeKind::Identifier);
        if (!ident->callParameters)
            return true;
        typeFunc = reinterpret_cast<TypeInfoFuncAttr*>(ip->b.pointer);
    }

    VectorNative<uint32_t> done;
    bool                   doneObjInterface = false;
    for (uint32_t i = pushParams.size() - 1; i != UINT32_MAX; i--)
    {
        uint32_t idx;
        if (typeFunc)
            idx = typeFunc->registerIdxToParamIdx(pushParams.size() - i - 1);
        else
            idx = pushParams.size() - i - 1;

        if (typeFunc && typeFunc->isClosure() && idx == 0)
            continue;
        const auto typeParam = typeFunc ? typeFunc->parameters[idx]->typeInfo : nullptr;

        // For an interface, we test the second register, which is the pointer to the 'itable'.
        // It is legit to have a null object with a table.
        if (typeParam && typeParam->isInterface())
        {
            if (!doneObjInterface)
            {
                doneObjInterface = true;
                continue;
            }
        }

        if (done.contains(idx))
            continue;
        done.push_back(idx);

        if (!typeParam || (typeParam->couldBeNull() && !typeParam->isNullable()))
        {
            SanityValue* ra = nullptr;
            SWAG_CHECK(STATE()->getRegister(ra, pushParams[i]));

            if (ra->isConstant() && !ra->reg.u64)
            {
                Utf8 msg;
                if (ip->op == ByteCodeOp::LambdaCall)
                    msg = formErr(Saf0030, "lambda");
                else if (typeFunc)
                    msg = formErr(Saf0030, typeFunc->declNode->token.cstr());
                else
                    msg = formErr(Saf0030, intrinsic.cstr());

                AstNode* locNode = nullptr;
                if (ip->node && ip->node->childCount() && ip->node->lastChild()->is(AstNodeKind::FuncCallParams))
                    locNode = ip->node->lastChild()->children[idx];
                else if (ip->node && ip->node->childCount() == 1)
                    locNode = ip->node->firstChild();

                const auto err = raiseError(STATE()->ip, msg, ra, locNode);
                if (!err)
                    continue;

                if (typeFunc && typeFunc->declNode && typeFunc->declNode->is(AstNodeKind::FuncDecl))
                {
                    const auto funcDecl = castAst<AstFuncDecl>(typeFunc->declNode, AstNodeKind::FuncDecl);
                    SWAG_ASSERT(funcDecl->parameters);

                    const auto childParam = funcDecl->parameters->children[idx];
                    if (childParam->isGeneratedMe())
                    {
                        msg = form("%s [[%s]] does not accept a null value as %s", "function", funcDecl->token.cstr(), "an implicit first argument");
                        err->addNote(funcDecl, funcDecl->getTokenName(), msg);
                    }
                    else
                    {
                        msg = form("%s [[%s]] does not accept a null value as %s", "function", funcDecl->token.cstr(), Naming::aNiceArgumentRank(idx + 1).cstr());
                        err->addNote(childParam, msg);
                    }
                }
                else if (!intrinsic.empty())
                {
                    err->addNote(form("%s [[%s]] does not accept a null value as %s", "intrinsic", intrinsic.cstr(), Naming::aNiceArgumentRank(idx + 1).cstr()));
                }

                return context.report(*err);
            }
        }
    }

    return true;
}

ByteCodeSanityState* ByteCodeSanity::newState(ByteCodeInstruction* fromIp, ByteCodeInstruction* startIp)
{
    const auto state = new ByteCodeSanityState;
    *state           = *STATE();

    context.states.push_back(state);
    state->fromIp = fromIp;
    state->ip     = startIp;
    state->parent = context.state;
    state->san    = this;
    return state;
}

bool ByteCodeSanity::process(ByteCode* bc)
{
    if (bc->isCompilerGenerated)
        return true;
    if (!bc->node)
        return true;
    if (!bc->node->token.sourceFile)
        return true;

    if (bc->node->hasAttribute(ATTRIBUTE_SANITY_OFF))
        return true;
    if (!bc->node->token.sourceFile->module->buildCfg.sanity && !bc->node->hasAttribute(ATTRIBUTE_SANITY_ON))
        return true;

    context.bc         = bc;
    context.sourceFile = bc->sourceFile;

    const auto state    = new ByteCodeSanityState;
    const auto funcDecl = castAst<AstFuncDecl>(context.bc->node, AstNodeKind::FuncDecl);
    state->stack.resize(funcDecl->stackSize);
    state->stackKind.resize(funcDecl->stackSize);
    state->regs.resize(context.bc->maxReservedRegisterRC);
    state->ip  = context.bc->out;
    state->san = this;
    context.states.push_back(state);

    for (uint32_t i = 0; i < context.states.size(); i++)
    {
        context.state = i;
        SWAG_CHECK(loop());
        if (i == context.states.size() - 1)
            break;
        for (uint32_t j = 0; j < context.bc->numInstructions; j++)
            context.bc->out[j].dynFlags.remove(BCID_SAN_PASS);
    }

    for (const auto s : context.states)
        delete s;

    return true;
}
