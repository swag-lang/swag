//#global testerror "Err0771"

#test
{
    let g: any = 66
    var r = 0
    switch g
    {
    case var ptr1: s32 where ptr1 == 66:
        r = ptr1
    }

    @assert(r == 66)
}
