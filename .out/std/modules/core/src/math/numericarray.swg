#global export
#global namespace math
using Swag

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

    #[safety("boundcheck", false), complete]
    func opAffect(using self, what: const [..] T)
    {
        Debug.assert(@countof(what) == N)
        loop N buf[@index] = what[@index]
    }

    #[safety("boundcheck", false)]
    func from(values: T...)->Self
    {
        Debug.assert(@countof(values) == N)
        var result: retval
        loop N result.buf[@index] = values[@index]
        return result
    }

    // Set all values
    #[safety("boundcheck", false)]
    func setAll(using self, value: T)
    {
        loop N buf[@index] = value
    }

    // Set all values
    #[safety("boundcheck", false)]
    func set(using self, values: T...)
    {
        Debug.assert(@countof(values) == N)
        loop N buf[@index] = values[@index]
    }

    // Add
    #[safety("boundcheck", false)]
    func add(using self, value: T)
    {
        loop N buf[@index] += value
    }

    // Multiply
    #[safety("boundcheck", false)]
    func mul(using self, value: T)
    {
        loop N buf[@index] *= value
    }

    // Multiply & add
    #[safety("boundcheck", false)]
    func mulAdd(using self, value1, value2: T)
    {
        loop N buf[@index] = (buf[@index] * value1) + value2
    }

    // Add
    #[safety("boundcheck", false)]
    func add(using self, value: Self)
    {
        loop N buf[@index] += value.buf[@index]
    }

    // Multiply
    #[safety("boundcheck", false)]
    func mul(using self, value: Self)
    {
        loop N buf[@index] *= value.buf[@index]
    }

    // Multiply & Add
    #[safety("boundcheck", false)]
    func mulAdd(using self, value1, value2: Self)
    {
        loop N buf[@index] = (buf[@index] * value1[@index]) + value2.buf[@index]
    }
}