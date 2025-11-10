//#[Swag.PrintBc]
func(T) insertionSort(x: const [..] T, cb: func||(T, T)->s32)
{
    @assert(x[0] == 1)
    @assert(x[1] == 2)
    //discard cb(0, 1)
}

//#[Swag.PrintBc]
func(T) sort(cb: func(T, T)->s32)
{
    insertionSort([1's32, 2], cb)
}

#test
{
    func toto(x, y: s32)->s32
    {
        return 0
    }

    sort(&toto)
}
