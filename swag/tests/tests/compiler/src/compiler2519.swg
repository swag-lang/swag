var t: struct {
    in:  const [..] char
} = @{[1'char,2,3,4]}

#test
{
    @assert(@countof(t.in) == 4)
    @assert(t.in[0] == 1)
    @assert(t.in[1] == 2)
    @assert(t.in[2] == 3)
    @assert(t.in[3] == 4)
}