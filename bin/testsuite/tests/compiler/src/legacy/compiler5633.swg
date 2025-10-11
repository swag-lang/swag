//#global skip
/*struct(T, N: u32) Arr
    where N > 0
{
    array: [N] T
}

#test
{
    //let arr: NonEmptyArray'(s32, 5) = [1, 2, 3, 4, 5]
    let arr: Arr'(s32, 0)
}*/

#global testerror "Err0078"
func(N: u32) toto()
    where N > 0
{
    var x: [N] s32
}

#test
{
    toto'0()
}
