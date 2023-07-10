#global #[Swag.Safety("", false)]

//#[Swag.PrintBc]
#test
{
    let a = Swag.F64.Nan
    let b = Swag.F64.Nan
    @assert(a != b)
}
