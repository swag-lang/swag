struct(T) Arr
{
}

impl Arr
{
    mtd(V) contains(value: V)
    {
        discard toto()
    }

    mtd toto()->const [..] T
    {
        #print @typeof(T).name
        return [{1, 2}]
    }
}

struct C
{
    x, y: s32
}

#test
{
    var arr1: Arr'C
    var arr2: Arr'{x, y: s32}

    discard arr2.toto()
    discard arr1.toto()
}