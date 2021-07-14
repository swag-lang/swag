struct A
{
}

var DROP: u32

impl A
{
    func opDrop(using self)
    {
        DROP += 1
    }
}

//#[Swag.PrintBc]
#test
{
    DROP = 0
    cpt := 0
    label outer
    {
        var x: A
        defer cpt = 1
        loop 10
            break outer
        @assert(false)
    }

    @assert(cpt == 1)
    @assert(DROP == 1)
}