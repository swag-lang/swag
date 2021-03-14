#[swag.inline]
func(T) toto() #selectif true
{
}

#[swag.inline]
func(T) toto() #selectif false
{
}

//#[swag.printbc]
#test
{
    toto's32()
}