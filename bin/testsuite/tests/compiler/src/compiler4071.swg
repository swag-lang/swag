#test
{
    var x = "false"
    let y = #ref &x
    @assert(#typeof(y) == #type &string)
    let z = @dataof(x)
    @assert(z[0] == 'f''u8)
}
