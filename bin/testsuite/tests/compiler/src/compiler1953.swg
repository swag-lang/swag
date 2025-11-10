#global skip
/*struct ST
{
    x: s32
}

var GG0: ST
var GG1: ST

impl ST
{
    func opEquals(const me, other: ST)->bool
    {
        return me.x == other.x
    }

    func opCmp(const me, other: ST)->s32
    {
        return me.x - other.x
    }

    func(op: string) opBinary(const me, other: ST)->ST
    {
        #if op == "+"
        {
            /*var res: ST
            res.x = me.x + other.x
            return res*/
            return ST{me.x + other.x}
        }
        #else
            return 0
    }

    func(op: string) opUnary(const me)->ST
    {
        #if op == "-"
        {
            var res: ST
            res.x = -me.x
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

    var res = ref0() + ref1()
    @assert(res.x == 2)

    var res1 = -ref0()
    @assert(res1.x == -1)
}*/