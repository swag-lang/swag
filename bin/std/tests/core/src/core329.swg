using Core, Core.StrConv

struct(T) AA
{
    arr: Array'T
}

impl AA
{
    mtd(op: string) opAssign(t: T)
    {
        #assert((op == "+="))
        me.arr.add(t)
    }

    mtd call(params: ...)
    {
        foreach fn in me.arr
        {
            #ast
            {
                var sb: StringBuilder
                let ts  = #typeof(T)

                sb.appendString("fn(")
                for i in @countof(ts.parameters)
                {
                    if i:
                        sb.appendString(",")
                    sb.appendFormat("params[%]", i)
                }

                sb.appendString(")\n")
                return sb.toString()
            }
        }
    }
}

var G = 0
#test
{
    var x: AA'func(s32, s32)
    x += func(x, y: s32)
    {
        G += x + y
    }
    x += func(x, y: s32)
    {
        G += x * y
    }
    x.call(2, 4)
    @assert(G == 14)
}
