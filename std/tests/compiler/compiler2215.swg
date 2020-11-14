alias DWORD = s32

struct A
{
    myX: s64 = 666
}

impl A
{
    func toto(using self, value: DWORD)
    {
        v := myX
        @print(v)
    }
}

#test
{
    var st: A
    st.toto(66)

    {
        xxx := 0
        while xxx < 2
        {
            //a := 666
            //a = a
            xxx += 1
        }
    }

    {
        aaa := 0
        xxx := 0
        while xxx < 2
        {
            //a := 666
            //a = a
            xxx += 1
        }
    }
}