#test
{
    var a:  [10] u8
    var ptr = &a[0]
    dref cast(*u32) ptr = 0
}
