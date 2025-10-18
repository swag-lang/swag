//#global skip
//#global testerror "Err0310"

/* salut la foule
    /* */
*/
#test
{
    alias Func = func()->s32
    let v: any = func() => 66

    switch v
    {
    case Func as fn:
        @assert(fn() == 66)
    }
}
