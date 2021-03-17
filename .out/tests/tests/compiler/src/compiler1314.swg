struct A
{
}

var DROP: u32

impl A
{
    func opDrop(using self)
    {
        DROP += 1
        //@print("drop\n")
    }
}

//#[swag.printbc]
#test
{
    DROP = 0
    cpt := 0
    label outer
    {
        var x: A
        defer cpt += 1
        loop 10
        {
            var y: A
            defer cpt += 1
            loop 10
            {       
                var z: A
                defer cpt += 1
                break outer
            }
        }
        
        @assert(false)
    }
    
    @assert(DROP == 3)
    @assert(cpt == 3)
}