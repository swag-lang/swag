using Core

#test
{
    var s: String = "abcd"
    var s1: String
    visit s
    {
        s1 += #alias0
    }

    @assert(s == s1)
}