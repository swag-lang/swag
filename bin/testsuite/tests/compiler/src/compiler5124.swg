#global testerror "Err0227"
#test
{
    var x = 0
    var ptr = cast(^u8) &x
    ptr = ptr + 1.0
}