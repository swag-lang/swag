using Core

//#[Swag.PrintBc]
#test
{
    var arr: Array'string
    arr.add("toto")
    arr.add("titi")
    @assert(@countof(arr) == 2)
}
