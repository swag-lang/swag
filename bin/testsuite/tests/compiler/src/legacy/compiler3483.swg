//#[Swag.PrintBc]
func(T) insertionSort(cb: func||(T, T)->s32)
{
    discard cb(0, 1)
}

//#[Swag.PrintBc]
func(T) sort(cb: func(T, T)->s32)
{
    insertionSort(cb)
}

#test
{
    func toto(x, y: s32)->s32
    {
        return 0
    }

    sort(&toto)
}
