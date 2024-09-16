#global skip
#global testerror "Err0390"

#[Swag.PrintBc]
#test
{
    let g: any = 66
    var r = 0
    switch g
    {
    case let ptr1 as s32 where ptr1 == 66:
        r = ptr1
    }

    @assert(r == 66)
}
