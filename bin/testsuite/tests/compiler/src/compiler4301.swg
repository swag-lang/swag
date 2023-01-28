#global testerror
#test
{
    x := 0
    ptr := cast(*u8) &x
    dref (1 + ptr) = 1
}