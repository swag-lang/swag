var G = 0

//#[swag.printbc]
func(T) toto(v: T)
#selectif
{
    return @typeof(T) == f32
}
{
    #assert @typeof(T) == f32
    G += 1
}
/*
func(T) toto(v: T)
{
    G += 2
}*/

#test
{
    toto'f32(1.0)
    //toto's32(1)
    //@assert(G == 3)
}