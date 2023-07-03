//#global skip
struct Rectangle{ x: s32; }

func create(parent: *void = null, id: string = null)
{
}

func create(parent: *Rectangle, name: string, position: Rectangle, id: string = null)
{
}

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    create(id = "titi")
}