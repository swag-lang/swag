//#global skip
struct Rectangle{ x: s32; }

#[Swag.Overload]
func(T) create(parent: nl *void = null, id: nl string = null)
{
    create(parent, "", {})
}

#[Swag.Overload]
func(T) create(parent: *void, name: string, position: Rectangle, id: nl string = null)
{
}

#test
{
    create's32(id: "titi")
}