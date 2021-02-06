var t: [] struct {
    in:  const [..] char
} = @[{[1'char,2,3,4]}]

#test
{
    @print(cast(s64) @countof(t[0].in))
    /*@assert(t.in[0] == 1)
    @assert(t.in[1] == 2)
    @assert(t.in[2] == 3)
    @assert(t.in[3] == 4)*/
}