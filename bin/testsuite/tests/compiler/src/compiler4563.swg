struct A {}
impl A
{
    mtd opDrop()
    {
        @assert(false)
    }
}

struct Err {}

func tata()->A
{
    return {}
}

func toto(b: bool) throw
{
    defer #err discard tata()
    if b do
        throw Err{}
}

#test
{
    assume toto(false)
}
