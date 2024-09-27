using Swag

#[ConstExpr]
{
    #[Inline]
    {
        func isInteger(t: nn typeinfo)     => t.flags & TypeInfoFlags.Integer
        func isFloat(t: nn typeinfo)       => t.flags & TypeInfoFlags.Float
        func isType(t: nn typeinfo)        => t.flags & TypeInfoFlags.PointerTypeInfo
        func isPointer(t: nn typeinfo)     => t.kind == TypeInfoKind.Pointer
        func isEnum(t: nn typeinfo)        => t.kind == TypeInfoKind.Enum
        func isStruct(t: nn typeinfo)      => t.kind == TypeInfoKind.Struct
        func isStaticArray(t: nn typeinfo) => t.kind == TypeInfoKind.Array
    }
}

//#[Swag.PrintBc]
#test
{
    #assert isInteger(s32)
}
