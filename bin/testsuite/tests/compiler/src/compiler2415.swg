var G = 0

#[Swag.Mixin]
func(T) toto(v: T)
{
    #run
    {
        G += 1
    }
}

#test
{
    toto'f32(1.0)
    toto'f32(1.0)
}