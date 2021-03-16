#ast "const tt: [] s64 = @[0, 1, 2, 3]"

#placeholder tt
#test
{
    visit s: tt
    {
        @assert(s == @index)
    }
}