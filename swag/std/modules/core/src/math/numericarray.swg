#global export
#global namespace math
using swag

struct(T, N: u32) NumericArray
{
    buf: [N] T
}

impl NumericArray
{
    #[swag.printbc]
    func set(using self, value: T)
    {
        @print("toto")
        #ast
        {
            var v: String
            loop N v += format.toString("buf[%] = %", @index, value)
            return v.toString()
        }
    }
}