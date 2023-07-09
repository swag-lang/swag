//#[Swag.PrintBc]
func toto(x: [..] s32)
{
    var a = &x[0]
    @assert(@typeof(a).name == "^s32")
    @assert(@typeof(a) == ^s32)
}

#test
{
    var x = [1]
    toto(x)
}