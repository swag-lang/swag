struct A {}

var DROP: u32

impl A
{
    func opDrop(me)
    {
        DROP += 1
    }
}

//#[Swag.PrintBc]
#test
{
    DROP = 0
    var cpt = 0
    #scope(outer)
    {
        var x: A
        defer cpt = 1
        for 10 do
            break to outer
        @assert(false)
    }

    @assert(cpt == 1)
    @assert(DROP == 1)
}
