#global #[Swag.Safety("", "", false)]

//#[Swag.PrintBc]
#test
{
    let a = Swag.F64.NaN
    let b = Swag.F64.NaN
    @assert(a != b)
}
