#global testerror

//#[Swag.ConstExpr]
func isStruct(t: typeinfo) => t.kind == Swag.TypeInfoKind.Struct

//#[Swag.PrintBc]
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