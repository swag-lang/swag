enum E: string
{
    RED = "RED"
}

#test
{
    let x: string = cast(string) E.RED
    @assert(x == "RED")
}
