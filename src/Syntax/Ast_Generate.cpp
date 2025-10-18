#include "pch.h"

#include "Naming.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/SemanticJob.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/Parser/Parser.h"
#include "Wmf/SourceFile.h"

bool Ast::generateOpEquals(SemanticContext* context, TypeInfo* typeLeft, TypeInfo* typeRight)
{
    const auto typeLeftStruct  = castTypeInfo<TypeInfoStruct>(typeLeft, TypeInfoKind::Struct);
    const auto typeRightStruct = castTypeInfo<TypeInfoStruct>(typeRight, TypeInfoKind::Struct);

    bool hasStruct = false;
    for (const auto f : typeLeftStruct->fields)
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
        if (typeLeft->hasFlag(TYPEINFO_GENERATED_OP_EQUALS))
        {
            context->result = ContextResult::NewChildren;
            return true;
        }

        typeLeft->addFlag(TYPEINFO_GENERATED_OP_EQUALS);
    }

    Utf8 content;

    content += form("impl %s {\n", typeLeftStruct->structName.cstr());
    content += form("mtd const opEquals(o: %s)->bool\n{\n", typeRightStruct->structName.cstr());
    if (!hasStruct)
    {
        content += form("return @memcmp(cast(const [*] void) me, cast(const [*] void) &o, #sizeof(%s)) == 0\n", typeLeftStruct->structName.cstr());
    }
    else
    {
        for (uint32_t i = 0; i < typeLeftStruct->fields.size(); i++)
        {
            const auto typeField = typeLeftStruct->fields[i];
            const auto leftN     = typeLeftStruct->fields[i]->name.cstr();
            const auto rightN    = typeRightStruct->fields[i]->name.cstr();

            if (typeField->typeInfo->isArray())
            {
                const auto typeArr = castTypeInfo<TypeInfoArray>(typeField->typeInfo, TypeInfoKind::Array);
                if (!typeArr->finalType->isStruct())
                    content += form("if @memcmp(&me.%s[0], &o.%s[0], #sizeof(%s)) != 0 do return false\n", leftN, rightN, leftN);
                else
                {
                    content += form("for i in me.%s do ", leftN);
                    content += form("if me.%s[i] != o.%s[i] do return false\n", leftN, rightN);
                }

                continue;
            }

            content += form("if me.%s != o.%s do return false\n", leftN, rightN);
        }

        content += "return true\n";
    }

    content += "}\n}";

    Parser parser;
    parser.setup(context, context->sourceFile->module, context->sourceFile, PARSER_DEFAULT);
    const auto structDecl = castAst<AstStruct>(typeLeft->declNode, AstNodeKind::StructDecl);
    AstNode*   result     = nullptr;
    SWAG_CHECK(parser.constructEmbeddedAst(content, structDecl, structDecl, CompilerAstKind::TopLevelInstruction, true, &result));

    result->addAlternativeScope(typeRightStruct->declNode->ownerScope);

    SWAG_ASSERT(result->is(AstNodeKind::Impl));
    SWAG_ASSERT(result->lastChild()->is(AstNodeKind::FuncDecl));

    const auto job = context->baseJob;
    job->nodes.push_back(result);
    context->result = ContextResult::NewChildren;
    return true;
}

bool Ast::generateMissingInterfaceFct(SemanticContext*            context,
                                      VectorNative<AstFuncDecl*>& mapItIdxToFunc,
                                      TypeInfoStruct*             typeStruct,
                                      const TypeInfoStruct*       typeBaseInterface,
                                      TypeInfoStruct*             typeInterface)
{
    const auto     node            = castAst<AstImpl>(context->node, AstNodeKind::Impl);
    const uint32_t numFctInterface = typeInterface->fields.size();

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

        const auto     missingNode       = typeInterface->fields[idx];
        const AstNode* defaultExportNode = nullptr;

        if (!itfRef.itf)
        {
            defaultExportNode = missingNode->declNode->extraPointer<AstNode>(ExtraPointerKind::ExportNode);
            if (defaultExportNode)
            {
                const auto defaultFunc = castAst<AstFuncDecl>(defaultExportNode, AstNodeKind::FuncDecl);
                if (!defaultFunc->content)
                    defaultExportNode = nullptr;
            }

            if (!defaultExportNode)
            {
                Diagnostic err{node, node->getTokenName(), formErr(Err0283, typeBaseInterface->name.cstr(), typeStruct->getDisplayNameC())};
                err.addNote(missingNode->declNode, missingNode->declNode->getTokenName(), form("missing function [[%s]]", missingNode->name.cstr()));
                return context->report(err);
            }
        }

        content += "func impl ";
        content += missingNode->name;

        content += "(me";
        const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(missingNode->typeInfo, TypeInfoKind::LambdaClosure);
        for (uint32_t i = 1; i < typeFunc->parameters.size(); i++)
        {
            content += ", ";
            const auto type = typeFunc->parameters[i]->typeInfo;
            type->computeScopedNameExport();
            content += form("p%d: %s", i, type->scopedNameExport.cstr());
        }
        content += ")";

        content += " => ";

        if (defaultExportNode)
        {
            content += typeBaseInterface->name;
            content += ".__default.";
            content += missingNode->name;
            content += form("(cast(*%s) me", typeBaseInterface->name.cstr());
            for (uint32_t i = 1; i < typeFunc->parameters.size(); i++)
            {
                content += ", ";
                content += form("p%d", i);
            }
        }
        else
        {
            content += "me.";
            content += itfRef.fieldRef;
            content += ".";
            content += typeBaseInterface->name;
            content += ".";
            content += missingNode->name;
            content += "(";
            for (uint32_t i = 1; i < typeFunc->parameters.size(); i++)
            {
                if (i != 1)
                    content += ", ";
                content += form("p%d", i);
            }
        }

        content += ")\n";
    }

    // We have generated methods, so restart
    if (!content.empty())
    {
        const uint32_t numChildren = node->childCount();

        Parser parser;
        parser.setup(context, context->sourceFile->module, context->sourceFile, PARSER_DEFAULT);
        SWAG_CHECK(parser.constructEmbeddedAst(content, node, node, CompilerAstKind::MissingInterfaceMtd, false));

        for (uint32_t i = numChildren; i < node->childCount(); i++)
            context->baseJob->nodes.push_back(node->children[i]);
        context->result = ContextResult::NewChildren;
        return true;
    }

    return true;
}
