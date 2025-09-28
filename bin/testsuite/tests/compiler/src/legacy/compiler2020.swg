func toto()->s64 => 666

#test
{
    var x: any
    x = toto()
    @assert(@kindof(x) == s64)
}