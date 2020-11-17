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

alias TOTO = [3] A

#[swag.printbc]
func toto1()->TOTO
{
    var result: TOTO
    result[0].x = 10
    result[1].x = 20
    result[2].x = 30
    return result
}

#test
{
    res := toto1()
    @assert(G == 3)
    @assert(res[0].x == 10)
    @assert(res[1].x == 20)
    @assert(res[2].x == 30)
}