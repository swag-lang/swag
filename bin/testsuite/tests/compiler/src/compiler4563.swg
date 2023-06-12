struct A {}
impl A
{
    mtd opDrop()
    {
        @assert(false)
    }
}

func tata()->A
{
    return {}
}

func toto(b: bool) throw
{
    defer(err) discard tata()
    if b
        throw "toto"
}

#test
{
    assume toto(false)
}