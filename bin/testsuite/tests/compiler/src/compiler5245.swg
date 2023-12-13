#test
{
    var x = 5
    let y = const ref &x
    @assert(@typeof(y) == #type const ref s32)
}