enum E : string
{
    RED = "RED"
}

#test
{
    var x: string = E.RED
    @assert(x == "RED")
}