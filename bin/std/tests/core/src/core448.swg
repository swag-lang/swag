using Core

#test
{
    var s:  String = "abcd"
    var s1: String
    foreach s
    {
        s1 += #alias0
    }

    @assert(s == s1)
}
