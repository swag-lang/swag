//#global skip
struct Rectangle { x: s32 }

#[Swag.Overload]
func(T) create(parent: *void = null, id: string = null)
{
    create(parent, "", {})
}

#[Swag.Overload]
func(T) create(parent: #null *void, name: string, position: Rectangle, id: string = null) {}

#test
{
    create's32(id: "titi")
}
