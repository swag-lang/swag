struct(T) Arr {}

var G = 0
impl Arr
{
    mtd(V) contains(value: V)
    {
        discard toto()
    }

    mtd toto()->const [..] T
    {
        if G == 1 do
            @assert(#typeof(T).name == "C")
        return [{1, 2}]
    }
}

struct C { x, y: s32 }

#test
{
    var arr1: Arr'C
    var arr2: Arr'{ x, y: s32 }

    discard arr2.toto()
    G = 1
    discard arr1.toto()
}
