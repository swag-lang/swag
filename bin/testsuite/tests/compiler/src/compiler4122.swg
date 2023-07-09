#global #[Swag.Safety("", false)]

//#[Swag.PrintBc]
#test
{
    var a = Swag.F64.Nan
    var b = Swag.F64.Nan
    @assert(a != b)
}
