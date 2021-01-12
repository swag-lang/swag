#global export
#global namespace math
using swag

struct(T, N: u32) NumericArray
{
    buf: [N] T
}

impl NumericArray
{
    //#[swag.printbc]
    func set(using self, value: T)
    {
        #ast
        {
            var v: String
            loop N
            {
                x := format.toString("buf[%] = value\n", @index)
                v += x.toString()
            }

            return v.toString()
        }
    }
}