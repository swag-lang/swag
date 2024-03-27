#global skip
var GG: s32 = 666

func ref()->const &s32 => GG

#test
{
    @assert(ref() == 666)
    @assert(ref() > 555)
    @assert(ref() < 777)
    @assert(ref() + 1 == 667)
}