struct A {}

var DROP: u32

impl A
{
    func opDrop(me)
    {
        DROP += 1
        //@print("drop\n")
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
        defer cpt += 1
        for 10
        {
            var y: A
            defer cpt += 1
            for 10
            {
                var z: A
                defer cpt += 1
                break to outer
            }
        }

        @assert(false)
    }

    @assert(DROP == 3)
    @assert(cpt == 3)
}
