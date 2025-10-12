struct E
{
    next: *E
}

func toto(edge: *E)->*E
{
    return edge.next
}

//#[Swag.PrintBc]
#[Swag.Optimize(.All, true)]
#test
{
    var s, t: E
    s.next = &t

    var e, f: *E
    e = &s

    e = toto(e)
    f = e
    @assert(e == f)
}
