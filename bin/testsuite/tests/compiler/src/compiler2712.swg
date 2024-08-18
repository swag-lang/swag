#ast "const tt: [] s64 = [0, 1, 2, 3]"

#placeholder tt
#test
{
    visit s in tt
    {
        @assert(s == #index)
    }
}