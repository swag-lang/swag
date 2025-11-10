//#[Swag.PrintBc]
#[Swag.Optimize(.All, false)]
#test
{
    let a = 6
    let b = 4
    var c = 0

    switch a
    {
    case 6 where b == 0:
        break
    case 6 where b == 4:
        c = 1
        break
    }

    @assert(c == 1)
}
