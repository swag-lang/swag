#global testerror "Err0530"
#test
{
    var x, y: s32
    var z = true ? cast(const &s32) &x : y
    @print(#typeof(z).name)
}