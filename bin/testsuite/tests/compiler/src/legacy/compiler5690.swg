//#global skip
//#global testerror "Err0401"

//#[Swag.PrintBc]
#test
{
    let v: any = {4, 5}

    var g = 0
    switch v
    {
    case { x: s32, y: s32 } as fn:
        g = 1 //@print(fn.x, " ", fn.y)
        break
    }

    @assert(g == 1)
}
