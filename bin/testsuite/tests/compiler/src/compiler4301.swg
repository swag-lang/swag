#global testerror "Err0248"
#test
{
    var x = 0
    var ptr = cast(*u8) &x
    dref (1 + ptr) = 1
}