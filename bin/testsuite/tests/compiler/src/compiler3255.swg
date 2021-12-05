#global skip
struct Rectangle{ x: s32; }

//#[Swag.Inline]
func(T) create(parent: *void = null, id: string = null)->*T
{
    return create(parent, "", @{})
}

//#[Swag.Inline]
/*func(T) create(parent: *void, position: Rectangle, id: string = null)->*T
{
    return create(parent, "", position)
}*/

func(T) create(parent: *void, name: string, position: Rectangle, id: string = null)->*T
{
    return null
}

#test
{
    discard create's32(id: "titi")
}