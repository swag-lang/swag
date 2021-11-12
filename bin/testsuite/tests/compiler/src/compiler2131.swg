//#global skip
func(T) toto(v: T)
{

}

struct(T) A
{

}

impl A
{
    func titi(self)
    {
        var x: s32
        toto's32(x)
    }
}

#test
{
    toto'u16(0)
    var v: A'bool
    v.titi()
}