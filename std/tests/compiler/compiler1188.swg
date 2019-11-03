attr T()-> "var"

struct TOTO
{
    #[T]
    {
        x: s32 = 4 
        y: s32 = 10
    }
}

#test
{
    var pt: TOTO
    @assert(pt.x == 4)
    @assert(pt.y == 10)
}