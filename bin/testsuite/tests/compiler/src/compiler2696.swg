#[Swag.Inline]
func(T) toto() #selectif true
{
}

#[Swag.Inline]
func(T) toto() #selectif false
{
}

//#[Swag.PrintBc]
#test
{
    toto's32()
}