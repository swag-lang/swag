func swap(x, y: *s32)
{
    let tmp = dref x
    dref x = dref y
    dref y = tmp
}

//#[Swag.PrintBc]
//#[Swag.Safety(.All, .All, false)]
//#[Swag.Optimize(.All, false)]
func nextPermutation(arr: [..] s32)
{
    swap(&arr[3], &arr[4])
}

#test
{
    var phase = [0, 1, 2, 3, 4]
    nextPermutation(phase)
}
