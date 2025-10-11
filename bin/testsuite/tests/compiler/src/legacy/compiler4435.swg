func toto(a: &string, b: &&string)
{
    a = b
    if a != b do
        @assert(false)
}

#test
{
    var a = "1"
    var b = "2"
    toto(&a, moveref &b)
    @assert(a == "2")
}
