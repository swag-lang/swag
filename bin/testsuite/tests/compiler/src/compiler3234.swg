//#global skip
struct AA
{
}

#[Swag.Inline]
func create1()
{
    create({})
}

func create(x: AA)
{
}

//#[Swag.PrintBc]
#test
{
    create1()
}
