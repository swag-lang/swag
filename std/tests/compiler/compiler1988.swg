func(T) sort(value: T, cb: func(T, T)->s32)
{
}

#test
{
    sort(0's32, @(x, y) => x - y)
}