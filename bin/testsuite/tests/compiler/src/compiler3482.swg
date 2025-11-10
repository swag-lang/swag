//#[Swag.Safety(.All, .All, false)]
//#[Swag.PrintBc]
func(T) insertionSort(values: [..] T, cb: func||(T, T)->s32)
{
    let buffer = @dataof(values)
    for var i = 1'u32; i < 2; i += 1
    {
        for var j = i; cb(buffer[j], buffer[j - 1]) < 0; j -= 1
        {
        }
    }
}

//#[Swag.Optimize(.All, false)]
//#[Swag.Safety(.All, .All, false)]
#test
{
    var x = [1, 2]

    func toto(x, y: s32)->s32
    {
        return 0
    }

    //@breakpoint()
    insertionSort(cast() x, &toto)
}
