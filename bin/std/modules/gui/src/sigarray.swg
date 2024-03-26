#global public
using Core

struct(T) SigArray
{
    arr:          Array'T
    disabled:     s32
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
        if disabled do
            return

        // Be sure the call has the right number of arguments
        Debug.assert(@countof(params) == @countof(@typeof(T).parameters))

        visit fn: arr
        {
            #ast
            {
                var sb: StrConv.StringBuilder
                let ts  = @typeof(T)

                sb.appendString("fn(")
                loop i: ts.parameters
                {
                    if i do
                        sb.appendString(",")
                    sb.appendFormat("cast(%) params[%]", ts.parameters[i].pointedType.fullname, i)
                }

                sb.appendString(")\n")
                return sb.toString()
            }
        }
    }
}
