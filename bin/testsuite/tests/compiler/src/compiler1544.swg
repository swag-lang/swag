using Swag

struct X
{
}

func toto(type: nn const* TypeInfo)
{
    @assert(type.kind == TypeInfoKind.Struct)
}

#test
{
    using Swag
    toto(#typeof(X))
}