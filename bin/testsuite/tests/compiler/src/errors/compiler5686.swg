//#global skip
//#global testerror "Err0689"

//#[Swag.PrintBc]
#test
{
    let g: any = 66
    var r  = 0
    switch g
    {
    case s32 as ptr1 where ptr1 == 66:
        r = ptr1
    }

    @assert(r == 66)
}
