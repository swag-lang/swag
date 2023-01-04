#global export
#global namespace Math
using Swag

struct(T, N: u32) NumericArray
{
    buf: [N] T
}

impl NumericArray
{
    mtdc opIndex(idx: uint)->T
    {
        return buf[idx]
    }

    mtdc opEquals(other: Self)->bool
    {
        loop N
        {
            if buf[@index] != other.buf[@index]
                return false
        }

        return true
    }

    #[Safety("bound", false), Complete]
    mtd opAffect(what: const [..] T)
    {
        Debug.assert(@countof(what) == N)
        loop N
            buf[@index] = what[@index]
    }

    #[Safety("bound", false)]
    func from(values: T...)->Self
    {
        Debug.assert(@countof(values) == N)
        var result: retval
        loop N
            result.buf[@index] = values[@index]
        return result
    }

    // Set all values
    #[Safety("bound", false)]
    mtd setAll(value: T)
    {
        loop N
            buf[@index] = value
    }

    // Set all values
    #[Safety("bound", false)]
    mtd set(values: T...)
    {
        Debug.assert(@countof(values) == N)
        loop N
            buf[@index] = values[@index]
    }

    // Add
    #[Safety("bound", false)]
    mtd add(value: T)
    {
        loop N
            buf[@index] += value
    }

    // Multiply
    #[Safety("bound", false)]
    mtd mul(value: T)
    {
        loop N
            buf[@index] *= value
    }

    // Multiply & add
    #[Safety("bound", false)]
    mtd mulAdd(value1, value2: T)
    {
        loop N
            buf[@index] = (buf[@index] * value1) + value2
    }

    // Add
    #[Safety("bound", false)]
    mtd add(value: Self)
    {
        loop N
            buf[@index] += value.buf[@index]
    }

    // Multiply
    #[Safety("bound", false)]
    mtd mul(value: Self)
    {
        loop N
            buf[@index] *= value.buf[@index]
    }

    // Multiply & Add
    #[Safety("bound", false)]
    mtd mulAdd(value1, value2: Self)
    {
        loop N
            buf[@index] = (buf[@index] * value1[@index]) + value2.buf[@index]
    }
}