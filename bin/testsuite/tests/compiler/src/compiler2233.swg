struct A
{
    x    = 1
}

var G = 0

impl A
{
    func opDrop(me)
    {
        //@print("drop\n")
    }

    func opPostMove(me)
    {
        G += 1
    }

    func opPostCopy(me)
    {
        //@assert(false)
    }
}

alias TOTO = [3] A

#[Swag.Inline]
func toto1()->TOTO
{
    var resultXX: TOTO
    resultXX[0].x = 10
    resultXX[1].x = 20
    resultXX[2].x = 30
    return resultXX
}

//#[Swag.PrintBc]
//#[Swag.Optimize(.All, true)]
#test
{
    var res = toto1()
    //@assert(G == 3)
    @assert(res[0].x == 10)
    @assert(res[1].x == 20)
    @assert(res[2].x == 30)
}
