using swag

struct X
{
}

func toto(type: const* TypeInfo)
{
    @assert(type.kind == TypeInfoKind.Struct)
}

#test
{
    using swag
    x := @typeof(X)
    x.toto()
}