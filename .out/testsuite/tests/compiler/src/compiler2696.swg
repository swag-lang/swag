#[Swag.inline]
func(T) toto() #selectif true
{
}

#[Swag.inline]
func(T) toto() #selectif false
{
}

//#[Swag.PrintBc]
#test
{
    toto's32()
}