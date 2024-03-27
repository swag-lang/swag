//#global skip
//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#test
{
    var a: {x, y: s32}
    var b: {x, y: s32}
    let c  = a == b
    @assert(c == true)
}
