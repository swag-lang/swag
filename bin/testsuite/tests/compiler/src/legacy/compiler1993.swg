struct A
{
    x:   s32 = 666
}

var GDrop = 0
var GMove = 0
var GInit = 0

impl A
{
    func opDrop(me)
    {
        GDrop += 1
    }

    func opPostMove(me)
    {
        GMove += 1
    }

    func opPostCopy(me)
    {
        @assert(false)
    }
}

//#[Swag.PrintBc]
#test
{
    {
        var x: A
        var y: A
        x = #nodrop #moveraw y
    }

    @assert(GMove == 1)
    @assert(GDrop == 2)
}
