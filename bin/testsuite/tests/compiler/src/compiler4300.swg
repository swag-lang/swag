#global testerror "Err0352"
#test
{
    var x = 0
    var ptr = cast(*u8) &x
    dref (ptr + 1) = 1
}