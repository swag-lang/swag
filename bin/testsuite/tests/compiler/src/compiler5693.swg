//#global skip
//#global testerror "Err0401"
//#global testerror "Saf0001"

#[Swag.Safety("", true)]
#test
{
    let v: any = {4, 5}

    switch v
    {
    case let fn as {x: s32, y: s32}:
        @assert(fn.x == 4)
        @assert(fn.y == 5)
        break
    }
}