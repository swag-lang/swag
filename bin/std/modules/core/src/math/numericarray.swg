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
        return .buf[idx]
    }

    mtd const opEquals(other: NumericArray)->bool
    {
        for i in NumericArray.N
        {
            if .buf[i] != other.buf[i] do
                return false
        }

        return true
    }

    mtd(op: string) const opBinary(other: NumericArray)->NumericArray
    {
        var result: retval
        for i in NumericArray.N
        {
            #if op == "+" do
                result.buf[i] = .buf[i] + other.buf[i]
            #elif op == "-" do
                result.buf[i] = .buf[i] - other.buf[i]
            #elif op == "*" do
                result.buf[i] = .buf[i] * other.buf[i]
            #elif op == "/" do
                result.buf[i] = .buf[i] / other.buf[i]
            #else do
                #error("unsupported binary operation [[" ++ op ++ "]]")
        }
        return result
    }

    #[Overload]
    mtd(op: string) opAssign(value: T)
    {
        for i in NumericArray.N
        {
            #if op == "+=" do
                .buf[i] += value
            #elif (op == "-=") do
                .buf[i] -= value
            #elif (op == "*=") do
                .buf[i] *= value
            #elif (op == "/=") do
                .buf[i] /= value
            #else do
                #error("unsupported assign operation [[" ++ op ++ "]]")
        }
    }

    #[Overload]
    func(op: string) opAssign(me, value: NumericArray)
    {
        for i in NumericArray.N
        {
            #if op == "+=" do
                .buf[i] += value.buf[i]
            #elif (op == "-=") do
                .buf[i] -= value.buf[i]
            #elif (op == "*=") do
                .buf[i] *= value.buf[i]
            #elif (op == "/=") do
                .buf[i] /= value.buf[i]
            #else do
                #error("unsupported assign operation [[" ++ op ++ "]]")
        }
    }

    #[Safety(.All, .BoundCheck, false), Complete, Overload]
    mtd opAffect(what: T)
    {
        for i in NumericArray.N do
            .buf[i] = what
    }

    #[Safety(.All, .BoundCheck, false), Complete, Overload]
    mtd opAffect(what: const [..] T)
    {
        Debug.assert(@countof(what) == NumericArray.N)
        for i in NumericArray.N do
            .buf[i] = what[i]
    }

    #[Safety(.All, .BoundCheck, false)]
    func from(values: T...)->NumericArray
    {
        Debug.assert(@countof(values) == NumericArray.N)
        var result: retval
        for NumericArray.N do
            result.buf[@index] = values[@index]
        return result
    }

    // Set all values
    #[Safety(.All, .BoundCheck, false)]
    mtd set(values: T...)
    {
        Debug.assert(@countof(values) == NumericArray.N)
        for i in NumericArray.N do
            .buf[i] = values[i]
    }

    // Multiply & add
    #[Safety(.All, .BoundCheck, false), Overload]
    mtd mulAdd(value1, value2: T)
    {
        for i in NumericArray.N do
            .buf[i] = (.buf[i] * value1) + value2
    }

    // Multiply & Add
    #[Safety(.All, .BoundCheck, false), Overload]
    mtd mulAdd(value1, value2: NumericArray)
    {
        for i in NumericArray.N do
            .buf[i] = (.buf[i] * value1[i]) + value2.buf[i]
    }
}
