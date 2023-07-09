attr Toto(x: s32)

//#[Toto]
//func titi() {}

#test
{
    var x = @typeof(Toto)
    @assert(x == Toto)
}