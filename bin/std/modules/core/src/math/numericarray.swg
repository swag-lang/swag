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

    mtd const opEquals(other: Self)->bool
    {
        loop i: N
        {
            if buf[i] != other.buf[i] do
                return false
        }

        return true
    }

    mtd(op: string) const opBinary(other: Self)->Self
    {
        var result: retval
        loop i: N
        {
            #if op == "+" #do
                result.buf[i] = buf[i] + other.buf[i]
            #elif op == "-" #do
                result.buf[i] = buf[i] - other.buf[i]
            #elif op == "*" #do
                result.buf[i] = buf[i] * other.buf[i]
            #elif op == "/" #do
                result.buf[i] = buf[i] / other.buf[i]
            #else #do
                #error "unsupported binary operation '" ++ op ++ "'"
        }
        return result
    }

    func(op: string) opAssign(using self, value: T)
    {
        loop i: N
        {
            #if op == "+=" #do
                buf[i] += value
            #elif (op == "-=") #do
                buf[i] -= value
            #elif (op == "*=") #do
                buf[i] *= value
            #elif (op == "/=") #do
                buf[i] /= value
            #else #do
                #error "unsupported assign operation '" ++ op ++ "'"
        }
    }

    func(op: string) opAssign(using self, value: Self)
    {
        loop i: N
        {
            #if op == "+=" #do
                buf[i] += value.buf[i]
            #elif (op == "-=") #do
                buf[i] -= value.buf[i]
            #elif (op == "*=") #do
                buf[i] *= value.buf[i]
            #elif (op == "/=") #do
                buf[i] /= value.buf[i]
            #else #do
                #error "unsupported assign operation '" ++ op ++ "'"
        }
    }

    #[Safety("boundcheck", false), Complete, Overload]
    mtd opAffect(what: T)
    {
        loop i: N do
            buf[i] = what
    }

    #[Safety("boundcheck", false), Complete, Overload]
    mtd opAffect(what: const [..] T)
    {
        Debug.assert(@countof(what) == N)
        loop i: N do
            buf[i] = what[i]
    }

    #[Safety("boundcheck", false)]
    func from(values: T...)->Self
    {
        Debug.assert(@countof(values) == N)
        var result: retval
        loop N do
            result.buf[#index] = values[#index]
        return result
    }

    // Set all values
    #[Safety("boundcheck", false)]
    mtd set(values: T...)
    {
        Debug.assert(@countof(values) == N)
        loop i: N do
            buf[i] = values[i]
    }

    // Multiply & add
    #[Safety("boundcheck", false), Overload]
    mtd mulAdd(value1, value2: T)
    {
        loop i: N do
            buf[i] = (buf[i] * value1) + value2
    }

    // Multiply & Add
    #[Safety("boundcheck", false), Overload]
    mtd mulAdd(value1, value2: Self)
    {
        loop i: N do
            buf[i] = (buf[i] * value1[i]) + value2.buf[i]
    }
}