#test
{
    x := 666
    var y: const& s32 = x
    @assert(y == 666)
    z := &y
    //@print(@typeof(z).name)
    //@error("toto", #location)
    @assert(false)
}