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
        return me.buf[idx]
    }

    mtd const opEquals(other: NumericArray)->bool
    {
        for i in NumericArray.N
        {
            if me.buf[i] != other.buf[i]:
                return false
        }

        return true
    }

    mtd(op: string) const opBinary(other: NumericArray)->NumericArray
    {
        var result: retval
        for i in NumericArray.N
        {
            #if op == "+":
                result.buf[i] = me.buf[i] + other.buf[i]
            #elif op == "-":
                result.buf[i] = me.buf[i] - other.buf[i]
            #elif op == "*":
                result.buf[i] = me.buf[i] * other.buf[i]
            #elif op == "/":
                result.buf[i] = me.buf[i] / other.buf[i]
            #else:
                #error("unsupported binary operation [[" ++ op ++ "]]")
        }
        return result
    }

    #[Overload]
    mtd(op: string) opAssign(value: T)
    {
        for i in NumericArray.N
        {
            #if op == "+=":
                me.buf[i] += value
            #elif (op == "-="):
                me.buf[i] -= value
            #elif (op == "*="):
                me.buf[i] *= value
            #elif (op == "/="):
                me.buf[i] /= value
            #else:
                #error("unsupported assign operation [[" ++ op ++ "]]")
        }
    }

    #[Overload]
    func(op: string) opAssign(me, value: NumericArray)
    {
        for i in NumericArray.N
        {
            #if op == "+=":
                me.buf[i] += value.buf[i]
            #elif (op == "-="):
                me.buf[i] -= value.buf[i]
            #elif (op == "*="):
                me.buf[i] *= value.buf[i]
            #elif (op == "/="):
                me.buf[i] /= value.buf[i]
            #else:
                #error("unsupported assign operation [[" ++ op ++ "]]")
        }
    }

    #[Safety("boundcheck", false), Complete, Overload]
    mtd opAffect(what: T)
    {
        for i in NumericArray.N:
            me.buf[i] = what
    }

    #[Safety("boundcheck", false), Complete, Overload]
    mtd opAffect(what: const [..] T)
    {
        Debug.assert(@countof(what) == NumericArray.N)
        for i in NumericArray.N:
            me.buf[i] = what[i]
    }

    #[Safety("boundcheck", false)]
    func from(values: T...)->NumericArray
    {
        Debug.assert(@countof(values) == NumericArray.N)
        var result: retval
        for NumericArray.N:
            result.buf[@index] = values[@index]
        return result
    }

    // Set all values
    #[Safety("boundcheck", false)]
    mtd set(values: T...)
    {
        Debug.assert(@countof(values) == NumericArray.N)
        for i in NumericArray.N:
            me.buf[i] = values[i]
    }

    // Multiply & add
    #[Safety("boundcheck", false), Overload]
    mtd mulAdd(value1, value2: T)
    {
        for i in NumericArray.N:
            me.buf[i] = (me.buf[i] * value1) + value2
    }

    // Multiply & Add
    #[Safety("boundcheck", false), Overload]
    mtd mulAdd(value1, value2: NumericArray)
    {
        for i in NumericArray.N:
            me.buf[i] = (me.buf[i] * value1[i]) + value2.buf[i]
    }
}
