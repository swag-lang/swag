//#global testerror "Saf0001"

#[Swag.Safety("", true)]
#test
{
    let v: any = {4, 5}

    let v1 = cast({x: s32, y: s32}) v
    @assert(v1.x == 4)
    @assert(v1.y == 5)

    /*switch v
    {
    case let fn: {x: s32, y: s32}:
        @print(fn.x, " ", fn.y)
        break
    }*/
}