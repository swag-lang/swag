using Swag

#[ConstExpr]
{
    #[inline]
    {
        func isInteger(t: typeinfo)     => t.flags & TypeInfoFlags.Integer
        func isFloat(t: typeinfo)       => t.flags & TypeInfoFlags.Float
        func isType(t: typeinfo)        => t.flags & TypeInfoFlags.TypeInfoPtr
        func isPointer(t: typeinfo)     => t.kind == TypeInfoKind.Pointer
        func isEnum(t: typeinfo)        => t.kind == TypeInfoKind.Enum
        func isStruct(t: typeinfo)      => t.kind == TypeInfoKind.Struct
        func isArray(t: typeinfo)       => t.kind == TypeInfoKind.Array
    }
}

//#[Swag.printbc]
#test
{
    #assert isInteger(s32)
}
