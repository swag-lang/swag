#global #[Swag.Safety("", false)]

//#[Swag.PrintBc]
#test
{
    var a = Swag.F32.Nan
    var b = Swag.F32.Nan
    var c = a == b
    @assert(c == false)
}

#test
{
    var a = Swag.F32.Nan
    var b = Swag.F32.Nan
    var c = a != b
    @assert(c == true)
}

#test
{
    #assert Swag.F32.Nan != Swag.F32.Nan
    var a = Swag.F32.Nan
    var c = a == a
    @assert(c == false)
}