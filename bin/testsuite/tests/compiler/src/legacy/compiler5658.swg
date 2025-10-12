//#global skip
//#[Swag.PrintBc]
#[Swag.Optimize(.All, false)]
#test
{
    let a = 1
    var b = 0

    switch a
    {
    case 0 where a == 0:
        b = 2
        break
    case 1 where a == 2:
        b = 1
        break
    }

    @assert(b == 0)
}
