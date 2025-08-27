func toto()->const [..] string
{
    return ["str0", "str1"]
}

#test
{
    let x = toto()[0]
    @assert(x == "str0")
    let y = toto()[1]
    @assert(y == "str1")
}
