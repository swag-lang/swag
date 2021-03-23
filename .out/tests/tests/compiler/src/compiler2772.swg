#test
{
    x := 2's8
    y := 1's8
    @assert(@min(x, y) == 1)
    @assert(@max(x, y) == 2)
    @assert(@min(-1's8, 10's8) == -1)
    @assert(@max(-1's8, 10's8) == 10)
}