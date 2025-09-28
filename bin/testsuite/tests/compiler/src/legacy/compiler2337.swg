using Swag

#[ConstExpr]
{
    #[Inline]
    {
        func isInteger(t: typeinfo)     => t.flags & TypeInfoFlags.Integer
        func isFloat(t: typeinfo)       => t.flags & TypeInfoFlags.Float
        func isType(t: typeinfo)        => t.flags & TypeInfoFlags.PointerTypeInfo
        func isPointer(t: typeinfo)     => t.kind == TypeInfoKind.Pointer
        func isEnum(t: typeinfo)        => t.kind == TypeInfoKind.Enum
        func isStruct(t: typeinfo)      => t.kind == TypeInfoKind.Struct
        func isStaticArray(t: typeinfo) => t.kind == TypeInfoKind.Array
    }
}

//#[Swag.PrintBc]
#test
{
    #assert(isInteger(s32))
}
