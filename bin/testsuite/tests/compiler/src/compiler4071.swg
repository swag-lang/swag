#test
{
    var x = "false"
    var y = ref &x
    @assert(@typeof(y) == #type ref string)
    var z = @dataof(x)
    @assert(z[0] == "f"'u8)
}