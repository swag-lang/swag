#global testerror "Err0294"
#test
{
    var x = 0
    var ptr = cast(*u8) &x
    dref (ptr + 1) = 1
}