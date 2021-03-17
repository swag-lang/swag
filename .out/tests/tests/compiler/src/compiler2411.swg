var G = 0

//#[swag.printbc]
func(T) toto(v: T) #selectif @typeof(T) == f32
{
    #assert @typeof(T) == f32
    G += 1
}

func(T) toto(v: T) #selectif @typeof(T) == s32
{
    #assert false
}

#test
{
    toto'f32(1.0)
    @assert(G == 1)
}