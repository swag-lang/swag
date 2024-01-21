#global testerror "Err0614"

#[Swag.Overload]
func(T) toto() #validif @typeof(T) == f32
{
}

#[Swag.Overload]
func(T) toto() #validif @typeof(T) == f64
{
}

#test
{
    toto's32()
}