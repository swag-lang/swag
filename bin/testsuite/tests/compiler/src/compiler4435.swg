func toto(a: ref string, b: moveref string)
{
    a = b
    if a != b
        @assert(false)
}

#test
{
    a := "1"
    b := "2"
    toto(&a, moveref &b)
    @assert(a == "2")
}