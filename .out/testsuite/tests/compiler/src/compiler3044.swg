/*struct(T: const [..] typeinfo) AA
{
}

impl AA
{
    /*method isIn(V: typeinfo)->bool
    {
        var ms: uint = 0
        visit p: T
            if p == V return true
        return false
    }*/

    method isIn()
    {
    }
}

#test
{
    var v: AA'(@[s32, bool])
    v.isIn()
}*/