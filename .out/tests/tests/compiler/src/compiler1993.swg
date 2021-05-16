struct A
{
    x: s32 = 666
}

var GDrop = 0
var GMove = 0
var GInit = 0

impl A
{
    func opDrop(self)
    {
        GDrop += 1
    }

    func opPostMove(self)
    {
        GMove += 1
    }

    func opPostCopy(self)
    {
        @assert(false)
    }
}

//#[swag.printbc]
#test
{
    {
        var x: A
        var y: A
        x =,nodrop,moveraw y
    }

    @assert(GMove == 1)
    @assert(GDrop == 2)
}