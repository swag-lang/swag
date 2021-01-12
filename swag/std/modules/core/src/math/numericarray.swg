#global export
#global namespace math
using swag

struct(T, N: u32) NumericArray
{
    buf: [N] T
}

impl NumericArray
{
    func opIndex(using self, idx: uint)->T
    {
        return buf[idx]
    }

    // Set all values
    #[swag.safety(false)]
    func set(using self, value: T)
    {
        #ast
        {
            var v: String
            loop N v += format.toString("buf[%] = value\n", @index)
            return v.toString()
        }
    }
}