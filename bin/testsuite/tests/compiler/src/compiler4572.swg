#global testerror "Err0643"
#test
{
    var x = [1'u8,1'u8,1'u8]
    var y = cast([..] u16) x
}