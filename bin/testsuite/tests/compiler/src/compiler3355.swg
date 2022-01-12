func toto(x: s32)
{
    y := #self.x
    @assert(y == 10)
}

#test
{
    toto(10)
}