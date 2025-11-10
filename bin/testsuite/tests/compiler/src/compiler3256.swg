//#global skip
struct Rectangle { x: s32 }

#[Swag.Overload]
func create(parent: *void = null, id: string = null) {}

#[Swag.Overload]
func create(parent: *Rectangle, name: string, position: Rectangle, id: string = null) {}

//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    create(id: "titi")
}
