//#global skip

func(T, R) map(v: T, cb: func(T)->R)->R
{
    #assert(T == s32)
    #assert(R == f32)
    return cb(v)
}

#test
{
    let res = map(1's32, func(x) => cast(f32) x)
}
