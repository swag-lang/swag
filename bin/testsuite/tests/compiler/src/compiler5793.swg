struct A { a, interior: bool }

var G: *A
func popBack()->*A
{
    return G
}

//#[Swag.PrintGenBc]
func meshClean()
{
    let t = popBack()
    if !t or t.interior:
        return

    t.interior = true
}