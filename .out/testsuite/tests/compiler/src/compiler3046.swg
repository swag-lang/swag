struct(T: const [..] typeinfo) AA
{
}

impl AA
{
    #[Swag.ConstExpr]
    func isIn(V: typeinfo)->bool
    {
        var ms: uint = 0
        visit p: T
            if p == V return true
        return false
    }

    method(TV) set(v: TV)
    {
        #assert(isIn(TV), "invalid type " ~ @stringof(TV))
    }
}

#test
{
    var v: AA'(@[s32, bool])
    v.set's32(0)
}