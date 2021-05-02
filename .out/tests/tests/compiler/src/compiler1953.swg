struct ST
{
    x: s32
}

var GG0: ST
var GG1: ST

impl ST
{
    func opEquals(using const self, other: Self)->bool
    {
        return self.x == other.x
    }

    func opCmp(using const self, other: Self)->s32
    {
        return self.x - other.x
    }

    func(op: string) opBinary(using const self, other: Self)->ST
    {
        #if op == "+"
        {
            /*var res: ST
            res.x = self.x + other.x
            return res*/
            return ST{self.x + other.x}
        }
        #else
            return 0
    }

    func(op: string) opUnary(using const self)->ST
    {
        #if op == "-"
        {
            var res: ST
            res.x = -self.x
            return res
        }
        #else
            return 0
    }
}

func ref0()->const &ST => GG0
func ref1()->const &ST => GG1

#test
{
    GG0.x = 1
    GG1.x = 1
    @assert(ref0() == ref1())
    @assert(ref0() >= ref1())
    @assert(ref0() <= ref1())

    res := ref0() + ref1()
    @assert(res.x == 2)

    res1 := -ref0()
    @assert(res1.x == -1)
}