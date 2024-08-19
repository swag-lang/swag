#global testerror "Err0222"
#test
{
    var x = 0
    var ptr = cast(^u8) &x
    ptr = ptr + 1.0
}