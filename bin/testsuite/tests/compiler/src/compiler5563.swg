//#global testerror "Err0539"

//#[Swag.PrintBc]
#test
{
    var x = 666
    var ptr0 = &x
    let ptr1 = &ptr0
    let val = @mkany(ptr1, *s32)
}