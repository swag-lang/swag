#global testerror "Err0177"
#test
{
    var x, y: s32
    var z = true ? cast(const ref s32) &x : y
    @print(@typeof(z).name)
}