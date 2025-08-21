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
    mtd const opIndex(idx: u64)->T
    {
        return buf[idx]
    }

    mtd const opEquals(other: NumericArray)->bool
    {
        for i in N
        {
            if buf[i] != other.buf[i]:
                return false
        }

        return true
    }

    mtd(op: string) const opBinary(other: NumericArray)->NumericArray
    {
        var result: retval
        for i in N
        {
            #if op == "+":
                result.buf[i] = buf[i] + other.buf[i]
            #elif op == "-":
                result.buf[i] = buf[i] - other.buf[i]
            #elif op == "*":
                result.buf[i] = buf[i] * other.buf[i]
            #elif op == "/":
                result.buf[i] = buf[i] / other.buf[i]
            #else:
                #error("unsupported binary operation [[" ++ op ++ "]]")
        }
        return result
    }

    #[Overload]
    func(op: string) opAssign(using me, value: T)
    {
        for i in N
        {
            #if op == "+=":
                buf[i] += value
            #elif (op == "-="):
                buf[i] -= value
            #elif (op == "*="):
                buf[i] *= value
            #elif (op == "/="):
                buf[i] /= value
            #else:
                #error("unsupported assign operation [[" ++ op ++ "]]")
        }
    }

    #[Overload]
    func(op: string) opAssign(using me, value: NumericArray)
    {
        for i in N
        {
            #if op == "+=":
                buf[i] += value.buf[i]
            #elif (op == "-="):
                buf[i] -= value.buf[i]
            #elif (op == "*="):
                buf[i] *= value.buf[i]
            #elif (op == "/="):
                buf[i] /= value.buf[i]
            #else:
                #error("unsupported assign operation [[" ++ op ++ "]]")
        }
    }

    #[Safety("boundcheck", false), Complete, Overload]
    mtd opAffect(what: T)
    {
        for i in N:
            buf[i] = what
    }

    #[Safety("boundcheck", false), Complete, Overload]
    mtd opAffect(what: const [..] T)
    {
        Debug.assert(@countof(what) == N)
        for i in N:
            buf[i] = what[i]
    }

    #[Safety("boundcheck", false)]
    func from(values: T...)->NumericArray
    {
        Debug.assert(@countof(values) == N)
        var result: retval
        for N:
            result.buf[@index] = values[@index]
        return result
    }

    // Set all values
    #[Safety("boundcheck", false)]
    mtd set(values: T...)
    {
        Debug.assert(@countof(values) == N)
        for i in N:
            buf[i] = values[i]
    }

    // Multiply & add
    #[Safety("boundcheck", false), Overload]
    mtd mulAdd(value1, value2: T)
    {
        for i in N:
            buf[i] = (buf[i] * value1) + value2
    }

    // Multiply & Add
    #[Safety("boundcheck", false), Overload]
    mtd mulAdd(value1, value2: NumericArray)
    {
        for i in N:
            buf[i] = (buf[i] * value1[i]) + value2.buf[i]
    }
}
