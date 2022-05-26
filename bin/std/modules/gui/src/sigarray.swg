#global public
using Core

struct(T) SigArray
{
    arr:        Array'T
    disabled:   s32
}

impl SigArray
{
    mtd(op: string) opAssign(fn: T)
    {
        #assert op == "+="
        arr.add(fn)
    }

    mtd disable()
    {
        disabled += 1
    }

    mtd enable()
    {
        Debug.assert(disabled > 0)
        disabled -= 1
    }

    mtd call(params: ...)
    {
        if disabled
            return
        visit fn: arr
        {
            #ast
            {
                var sb: StrConv.StringBuilder
                ts := @typeof(T)

                sb.appendString("fn(")
                loop i: ts.parameters
                {
                    if i sb.appendString(",")
                    sb.appendFormat("params[%]", i)
                }

                sb.appendString(")\n")
                return sb.toString()
            }
        }
    }
}