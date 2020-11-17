struct A
{
    x = 1
}

var G = 0

impl A
{
    func opDrop(using self)
    {
        @print("drop\n")
    }

    func opPostMove(using self)
    {
        G += 1
    }

    func opPostCopy(using self)
    {
        @assert(false)
    }
}

#[swag.printbc]
#test
{
    var resultXX: [3] A
}