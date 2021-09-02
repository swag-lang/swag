func(T) titi(v: T, cb: func(T)->T)
{
    @assert(cb(v) == v)
}

func lambda(x: s32) => x

#test
{
    titi(10's32, &lambda)
    titi(10's32, @(x: s32) => x)
}