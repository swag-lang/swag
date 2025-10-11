//#[Swag.PrintBc]
func insertionSort(cb: func||(s32, s32)->s32)
{
    discard cb(0, 1)
}

//#[Swag.PrintBc]
func sort(cb: func(s32, s32)->s32)
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
