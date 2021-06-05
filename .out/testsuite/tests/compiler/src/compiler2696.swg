#[Swag.inline]
func(T) toto() #selectif true
{
}

#[Swag.inline]
func(T) toto() #selectif false
{
}

//#[Swag.printbc]
#test
{
    toto's32()
}