using Swag

struct X
{
}

func toto(type: const* TypeInfo)
{
    @assert(type.kind == TypeInfoKind.Struct)
}

#test
{
    using Swag
    x := @typeof(X)
    x.toto()
}