#pragma once
struct TypeInfoFuncAttr;
struct TypeInfoStruct;
struct SymbolMatchContext;

namespace Match
{
    void match(TypeInfoFuncAttr* typeFunc, SymbolMatchContext& context);
    void match(TypeInfoStruct* typeStruct, SymbolMatchContext& context);
}; // namespace Match