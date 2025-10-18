#global testerror "Err0403"

struct(T, N: u32) Arr
        where N > 0
{
    array: [N] T
}

#test
{
    //let arr: NonEmptyArray'(s32, 5) = [1, 2, 3, 4, 5]
    let arr: Arr'(s32, 0)
}
