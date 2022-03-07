#global public
using Core

struct(T) SigArray
{
    arr: Array'T
}

impl SigArray
{
    mtd(op: string) opAssign(fn: T)
    {
        #assert op == "+="
        arr.add(fn)
    }

    mtd call(params: ...)
    {
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
                return sb.moveToString()
            }
        }
    }
}