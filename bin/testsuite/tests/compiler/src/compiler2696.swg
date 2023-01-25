#[Swag.Inline]
func(T) toto() #selectifonce true
{
}

#[Swag.Inline]
func(T) toto() #selectifonce false
{
}

//#[Swag.PrintBc]
#test
{
    toto's32()
}