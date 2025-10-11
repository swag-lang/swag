func(T) sort(values: const [..] T, cb: func||(T, T)->s32) {}

#test
{
    sort([1's32, 2], func(x, y) => x <=> y)
}
