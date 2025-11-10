#global testerror "Err0645"
#test
{
    var x   = 0
    var ptr = cast([*] u8) &x
    ptr = ptr + 1.0
}
