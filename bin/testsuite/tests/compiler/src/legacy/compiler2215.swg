alias DWORD = s32

struct A
{
    myX: s64 = 666
}

impl A
{
    func toto(me, value: DWORD)->A
    {
        var result: retval
        result.myX = 0xAA's64
        return result
        //@print(v)
    }
}

#test
{
    var st: A
    var ew  = st.toto(66)

    {
        var xxx = 0
        while xxx < 2
        {
            //var a = 666
            //a = a
            xxx += 1
        }
    }

    {
        var aaa = 0
        var xxx = 0
        while xxx < 2
        {
            //var a = 666
            //a = a
            xxx += 1
        }
    }
}
