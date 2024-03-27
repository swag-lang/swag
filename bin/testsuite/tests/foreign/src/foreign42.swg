public func call42(x: string, fn: func(u64, string, u64)->u64)->u64
{
    return fn(3, x, 666)
}