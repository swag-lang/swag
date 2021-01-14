var G = 0

#[swag.mixin]
func(T) toto(v: T)
{
    #run
    {
        @print(@typeof(T).name)
    }
}

#test
{
    toto'f32(1.0)
    toto'f32(1.0)
}