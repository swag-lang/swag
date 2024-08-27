/* salut la foule
    /* */
*/
#test
{
    typealias Func = func()->s32
    let v: any = func() => 66

    switch v
    {
    case let fn: Func:
        @assert(fn() == 66)
    }
}