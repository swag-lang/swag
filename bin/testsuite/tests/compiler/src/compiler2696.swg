#[Swag.Inline]
func(T) toto() #validif true
{
}

#[Swag.Inline]
func(T) toto() #validif false
{
}

//#[Swag.PrintBc]
#test
{
    toto's32()
}