//#[Swag.PrintBc]
#test
{
    var array: [4] u32 = [10, 20, 30, 40]
    let slice: [..] u32 = @mkslice(&array[0] + 2, 2)
    slice[0] = 314
}
