struct A {}
impl A
{
    mtd opDrop()
    {
        @assert(false)
    }
}

struct Err{}


func tata()->A
{
    return {}
}

func toto(b: bool) throw
{
    defer(err) discard tata()
    if b:
        throw Err{}
}

#test
{
    assume toto(false)
}