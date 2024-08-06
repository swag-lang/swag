#global testerror "Err0339"
#test
{
    var x = 0
    var ptr = cast(^u8) &x
    ptr = ptr + 1.0
}