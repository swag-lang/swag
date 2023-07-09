#global #[Swag.Safety("", false)]

//#[Swag.PrintBc]
#test
{
    var a = Swag.F64.Nan
    var b = Swag.F64.Nan
    var c = a == b
    @assert(c == false)
}

#test
{
    var a = Swag.F64.Nan
    var b = Swag.F64.Nan
    var c = a != b
    @assert(c == true)
}

#test
{
    #assert Swag.F64.Nan != Swag.F64.Nan
    var a = Swag.F64.Nan
    var c = a == a
    @assert(c == false)
}