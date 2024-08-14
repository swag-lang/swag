#global testerror "Err0507"

#[Swag.Overload]
func(T) toto() where @typeof(T) == f32
{
}

#[Swag.Overload]
func(T) toto() where @typeof(T) == f64
{
}

#test
{
    toto's32()
}