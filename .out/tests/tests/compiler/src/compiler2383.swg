#global testerror

//#[Swag.constexpr]
func isStruct(t: typeinfo) => t.kind == Swag.TypeInfoKind.Struct

//#[Swag.printbc]
func(T) toto()
{
    var x: T
    v := isStruct(T) ? cast(const &T) x : x
    @print(@typeof(v).name)
}

#test
{
    toto's32()
}