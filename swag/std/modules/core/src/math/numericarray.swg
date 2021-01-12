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

    #[safety(false), complete]
    func opAffect(using self, slice: const [..] T)
    {
        debug.assert(@countof(slice) == N)
        loop N buf[@index] = slice[@index]
    }

    // Set all values
    #[safety(false)]
    func set(using self, value: T)
    {
        loop N buf[@index] = value
    }

    // Add one value
    #[safety(false)]
    func add(using self, value: T)
    {
        loop N buf[@index] += value
    }
}