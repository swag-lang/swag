#global testerror

//#[swag.constexpr]
func isStruct(t: typeinfo) => t.kind == swag.TypeInfoKind.Struct

//#[swag.printbc]
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