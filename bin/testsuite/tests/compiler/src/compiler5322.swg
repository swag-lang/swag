//#global skip

func(T, R) map(v: T, cb: nn func(T)->R)->R
{
    return cb(v)
}

#test
{
    let res = map(1's32, func(x)->bool => x != 0)
}