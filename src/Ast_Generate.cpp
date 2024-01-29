#include "pch.h"
#include "Ast.h"
#include "Diagnostic.h"
#include "Parser.h"
#include "SemanticJob.h"
#include "SourceFile.h"
#include "TypeManager.h"

bool Ast::generateOpEquals(SemanticContext* context, TypeInfo* typeLeft, TypeInfo* typeRight)
{
    auto typeLeftStruct  = CastTypeInfo<TypeInfoStruct>(typeLeft, TypeInfoKind::Struct);
    auto typeRightStruct = CastTypeInfo<TypeInfoStruct>(typeRight, TypeInfoKind::Struct);

    bool hasStruct = false;
    for (auto f : typeLeftStruct->fields)
    {
        if (f->typeInfo->isStruct() || f->typeInfo->isArrayOfStruct())
        {
            hasStruct = true;
            break;
        }
    }

    // Be sure another thread does not do the same thing
    {
        ScopedLock lk(typeLeft->mutex);
        if (typeLeft->flags & TYPEINFO_GENERATED_OPEQUALS)
        {
            context->result = ContextResult::NewChilds;
            return true;
        }

        typeLeft->flags |= TYPEINFO_GENERATED_OPEQUALS;
    }

    Utf8 content;

    content += Fmt("impl %s {\n", typeLeftStruct->structName.c_str());
    content += Fmt("mtd const opEquals(o: %s)->bool\n{\n", typeRightStruct->structName.c_str());
    if (!hasStruct)
    {
        content += "return @memcmp(cast(const ^void) self, cast(const ^void) &o, @sizeof(Self)) == 0\n";
    }
    else
    {
        for (size_t i = 0; i < typeLeftStruct->fields.size(); i++)
        {
            auto typeField = typeLeftStruct->fields[i];
            auto leftN     = typeLeftStruct->fields[i]->name.c_str();
            auto rightN    = typeRightStruct->fields[i]->name.c_str();

            if (typeField->typeInfo->isArray())
            {
                auto typeArr = CastTypeInfo<TypeInfoArray>(typeField->typeInfo, TypeInfoKind::Array);
                if (!typeArr->finalType->isStruct())
                    content += Fmt("if @memcmp(&%s[0], &o.%s[0], @sizeof(%s)) != 0 do return false\n", leftN, rightN, leftN);
                else
                {
                    content += Fmt("loop i: %s do ", leftN);
                    content += Fmt("if %s[i] != o.%s[i] do return false\n", leftN, rightN);
                }

                continue;
            }

            content += Fmt("if %s != o.%s do return false\n", leftN, rightN);
        }

        content += "return true\n";
    }

    content += "}\n}";

    Parser parser;
    parser.setup(context, context->sourceFile->module, context->sourceFile);
    auto     structDecl = CastAst<AstStruct>(typeLeft->declNode, AstNodeKind::StructDecl);
    AstNode* result     = nullptr;
    SWAG_CHECK(parser.constructEmbeddedAst(content, structDecl, structDecl, CompilerAstKind::TopLevelInstruction, true, &result));

    result->addAlternativeScope(typeRightStruct->declNode->ownerScope);

    SWAG_ASSERT(result->kind == AstNodeKind::Impl);
    SWAG_ASSERT(result->childs.back()->kind == AstNodeKind::FuncDecl);

    auto job = context->baseJob;
    job->nodes.push_back(result);
    context->result = ContextResult::NewChilds;
    return true;
}

bool Ast::generateMissingInterfaceFct(SemanticContext* context, VectorNative<AstFuncDecl*>& mapItIdxToFunc, TypeInfoStruct* typeStruct, TypeInfoStruct* typeBaseInterface, TypeInfoStruct* typeInterface)
{
    auto     node            = CastAst<AstImpl>(context->node, AstNodeKind::Impl);
    uint32_t numFctInterface = (uint32_t) typeInterface->fields.size();
    auto     typeInfo        = node->identifier->typeInfo;

    Vector<const Diagnostic*> notes;
    InterfaceRef              itfRef;
    bool                      doneItfRef = false;
    Utf8                      content;
    for (uint32_t idx = 0; idx < numFctInterface; idx++)
    {
        // Function is there
        if (mapItIdxToFunc[idx])
            continue;

        if (!doneItfRef)
        {
            doneItfRef = true;
            SWAG_CHECK(TypeManager::collectInterface(context, typeStruct, typeBaseInterface, itfRef, true));
            YIELD();
        }

        auto missingNode = typeInterface->fields[idx];
        if (!itfRef.itf)
        {
            Diagnostic diag{node, node->getTokenName(), Fmt(Err(Err0129), typeBaseInterface->name.c_str(), typeInfo->getDisplayNameC())};
            auto       note = Diagnostic::note(missingNode->declNode, missingNode->declNode->getTokenName(), Fmt("missing [[%s]]", missingNode->name.c_str()));
            return context->report(diag, note);
        }

        content += "func impl ";
        content += missingNode->name;

        content += "(using self";
        auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(missingNode->typeInfo, TypeInfoKind::LambdaClosure);
        for (size_t i = 1; i < typeFunc->parameters.size(); i++)
        {
            content += ", ";
            auto type = typeFunc->parameters[i]->typeInfo;
            type->computeScopedNameExport();
            content += Fmt("p%d: %s", i, type->scopedNameExport.c_str());
        }
        content += ")";

        content += " => ";
        content += itfRef.fieldRef;
        content += ".";
        content += typeBaseInterface->name;
        content += ".";
        content += missingNode->name;

        content += "(";
        for (size_t i = 1; i < typeFunc->parameters.size(); i++)
        {
            if (i != 1)
                content += ",";
            content += Fmt("p%d", i);
        }
        content += ")\n";
    }

    // We have generated methods, so restart
    if (!content.empty())
    {
        int numChilds = (int) node->childs.size();

        Parser parser;
        parser.setup(context, context->sourceFile->module, context->sourceFile);
        SWAG_CHECK(parser.constructEmbeddedAst(content, node, node, CompilerAstKind::MissingInterfaceMtd, true));

        for (size_t i = numChilds; i < node->childs.size(); i++)
            context->baseJob->nodes.push_back(node->childs[i]);
        context->result = ContextResult::NewChilds;
        return true;
    }

    return true;
}
