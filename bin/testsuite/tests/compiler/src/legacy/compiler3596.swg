//#global testerror "Err0024"

#[Swag.Overload]
func(T) toto(x: T) {}

#test
{
    toto's32(0)
    toto'f32(0)
    toto(0)
}
