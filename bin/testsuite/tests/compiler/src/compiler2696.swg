#[Swag.Inline, Swag.Overload]
func(T) toto()
    #validif true
{
}

#[Swag.Inline, Swag.Overload]
func(T) toto()
    #validif false
{
}

//#[Swag.PrintBc]
#test
{
    toto's32()
}
