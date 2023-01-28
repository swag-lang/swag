#global testerror
#test
{
    x := 0
    ptr := cast(*u8) &x
    dref (ptr + 1) = 1
}