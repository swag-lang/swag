using Swag

struct A {}

func get()->A
{
    return {}
}

func toto(x: A) {}

//#[Swag.PrintBc]
#test
{
    toto(get())
}
