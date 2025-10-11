using Swag

struct(T) NumericArray
{
    buf: T
}

impl NumericArray
{
    func(op: string) opAssign(me, value: NumericArray) {}
}

#test
{
    var v1: NumericArray's32
    let v2: NumericArray's32
    v1 += v2
}
