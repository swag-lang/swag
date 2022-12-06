//#[Swag.PrintBc]
func toto(x: [..] s32)
{
    a := &x[0]
    @assert(@typeof(a).name == "^s32")
    @assert(@typeof(a) == ^s32)
}

#test
{
    x := [1]
    toto(x)
}