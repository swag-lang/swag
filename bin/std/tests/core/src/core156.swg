using Core

//#[Swag.PrintBc]
#test
{
    let x = ".5"
    let y = try StrConv.parseF64(x)
    @assert(y.value == 0.5)
}
