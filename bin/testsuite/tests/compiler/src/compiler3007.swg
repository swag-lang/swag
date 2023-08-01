struct(T) Toto
{
    const Max = @countof(T)
}

#test
{
    typealias TT = Toto'Swag.TypeInfoKind
    var v: TT
    @assert(@countof(Swag.TypeInfoKind) == TT.Max)
    @assert(@countof(Swag.TypeInfoKind) == v.Max)
}