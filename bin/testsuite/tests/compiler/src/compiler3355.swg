func toto(x: s32)
{
    var y = #self.x
    @assert(y == 10)
}

#test
{
    toto(10)
}