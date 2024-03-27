//#global skip

func(T, R) map(v: T, cb: func(T)->R)->R
{
    return cb(v)
}

#test
{
    let res = map(1's32, func(x)->bool => x != 0)
}
