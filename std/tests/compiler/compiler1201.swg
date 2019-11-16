enum E : string
{
    RED = "RED"
}

#test
{
    var x: string = cast(string) E.RED
    @assert(x == "RED")
}