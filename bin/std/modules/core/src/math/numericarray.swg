#global export
#global namespace Math
using Swag

struct(T, N: u32) NumericArray
{
    buf: [N] T
}

impl NumericArray
{
    #[Inline]
    mtdc opIndex(idx: u64)->T
    {
        return buf[idx]
    }

    mtdc opEquals(other: Self)->bool
    {
        loop i: N
        {
            if buf[i] != other.buf[i]
                return false
        }

        return true
    }

    mtdc(op: string) opBinary(other: Self)->Self
    {
        var result: retval
        loop i: N
        {
            #if op == "+"
                result.buf[i] = buf[i] + other.buf[i]
            #elif op == "-"
                result.buf[i] = buf[i] - other.buf[i]
            #elif op == "*"
                result.buf[i] = buf[i] * other.buf[i]
            #elif op == "/"
                result.buf[i] = buf[i] / other.buf[i]
            #else
                #error "unsupported binary operation '" ++ op ++ "'"
        }
        return result
    }

    func(op: string) opAssign(using self, value: T)
    {
        loop i: N
        {
            #if op == "+="
                buf[i] += value
            #else #if (op == "-=")
                buf[i] -= value
            #else #if (op == "*=")
                buf[i] *= value
            #else #if (op == "/=")
                buf[i] /= value
            #else
                #error "unsupported assign operation '" ++ op ++ "'"
        }
    }

    func(op: string) opAssign(using self, value: Self)
    {
        loop i: N
        {
            #if op == "+="
                buf[i] += value.buf[i]
            #else #if (op == "-=")
                buf[i] -= value.buf[i]
            #else #if (op == "*=")
                buf[i] *= value.buf[i]
            #else #if (op == "/=")
                buf[i] /= value.buf[i]
            #else
                #error "unsupported assign operation '" ++ op ++ "'"
        }
    }

    #[Safety("boundcheck", false), Complete]
    mtd opAffect(what: T)
    {
        loop i: N
            buf[i] = what
    }

    #[Safety("boundcheck", false), Complete]
    mtd opAffect(what: const [..] T)
    {
        Debug.assert(@countof(what) == N)
        loop i: N
            buf[i] = what[i]
    }

    #[Safety("boundcheck", false)]
    func from(values: T...)->Self
    {
        Debug.assert(@countof(values) == N)
        var result: retval
        loop N
            result.buf[@index] = values[@index]
        return result
    }

    // Set all values
    #[Safety("boundcheck", false)]
    mtd set(values: T...)
    {
        Debug.assert(@countof(values) == N)
        loop i: N
            buf[i] = values[i]
    }

    // Multiply & add
    #[Safety("boundcheck", false)]
    mtd mulAdd(value1, value2: T)
    {
        loop i: N
            buf[i] = (buf[i] * value1) + value2
    }

    // Multiply & Add
    #[Safety("boundcheck", false)]
    mtd mulAdd(value1, value2: Self)
    {
        loop i: N
            buf[i] = (buf[i] * value1[i]) + value2.buf[i]
    }
}