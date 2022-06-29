attr Toto(x: s32)

//#[Toto]
//func titi() {}

#test
{
    x := @typeof(Toto)
    @assert(x == Toto)
}