func(T) toto()
{
    var x: T
    v := T == s32 ? cast(const &T) x : x
    @assert(@typeof(v).name == "const &s32")
}

#test
{
    toto's32()
}