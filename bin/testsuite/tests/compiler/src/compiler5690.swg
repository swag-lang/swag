#global testerror "Err0390"

//#[Swag.PrintBc]
#test
{
    let v: any = {4, 5}

    var g = 0
    switch v
    {
    case let fn as {x: s32, y: s32}:
        g = 1//@print(fn.x, " ", fn.y)
        break
    }

    @assert(g == 1)
}