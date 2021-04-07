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

    func opEquals(using self, other: Self)->bool
    {
        loop N
        {
            if buf[@index] != other.buf[@index]
                return false
        }

        return true
    }

    #[safety("bc", false), complete]
    func opAffect(using self, what: const [..] T)
    {
        debug.assert(@countof(what) == N)
        loop N buf[@index] = what[@index]
    }

    #[safety("bc", false)]
    func from(values: T...)->Self
    {
        debug.assert(@countof(values) == N)
        var result: retval
        loop N result.buf[@index] = values[@index]
        return result
    }

    // Set all values
    #[safety("bc", false)]
    func setAll(using self, value: T)
    {
        loop N buf[@index] = value
    }

    // Set all values
    #[safety("bc", false)]
    func set(using self, values: T...)
    {
        debug.assert(@countof(values) == N)
        loop N buf[@index] = values[@index]
    }

    // Add
    #[safety("bc", false)]
    func add(using self, value: T)
    {
        loop N buf[@index] += value
    }

    // Multiply
    #[safety("bc", false)]
    func mul(using self, value: T)
    {
        loop N buf[@index] *= value
    }

    // Multiply & add
    #[safety("bc", false)]
    func mulAdd(using self, value1, value2: T)
    {
        loop N buf[@index] = (buf[@index] * value1) + value2
    }

    // Add
    #[safety("bc", false)]
    func add(using self, value: Self)
    {
        loop N buf[@index] += value.buf[@index]
    }

    // Multiply
    #[safety("bc", false)]
    func mul(using self, value: Self)
    {
        loop N buf[@index] *= value.buf[@index]
    }

    // Multiply & Add
    #[safety("bc", false)]
    func mulAdd(using self, value1, value2: Self)
    {
        loop N buf[@index] = (buf[@index] * value1[@index]) + value2.buf[@index]
    }
}