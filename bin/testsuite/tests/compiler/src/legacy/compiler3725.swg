attr Toto(x: s32)

//#[Toto]
//func titi() {}

#test
{
    let x = #typeof(Toto)
    @assert(x == Toto)
}
