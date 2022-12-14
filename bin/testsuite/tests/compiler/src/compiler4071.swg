#test
{
    x := "false"
    y := ref &x
    @assert(@typeof(y) == #type ref string)
    z := @dataof(x)
    @assert(z[0] == "f"'u8)
}