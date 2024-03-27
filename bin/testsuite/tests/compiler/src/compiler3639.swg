var G = 0

#[Swag.Overload]
func(T) toto(x: const [..] T)
{
    G += 2
}

#[Swag.Overload]
func(T) toto(x: [4] T)
{
    G += 1
}

#test
{
    var a: [4] s32
    toto(a)
}
