func(T) sort(values: const [..] T, cb: closure(T, T)->s32)
{
}

#test
{
    sort([1's32, 2], func(x, y) => x <=> y)
}