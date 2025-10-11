//#global skip
struct AA {}

#[Swag.Inline]
func create1()
{
    create({})
}

func create(x: AA)
{
    //    @print("x")
}

//#[Swag.PrintBc]
#test
{
    create1()
}
