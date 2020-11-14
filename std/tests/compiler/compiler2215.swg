struct A
{
    x: s64 = 666
}

impl A
{
    func toto(using self, value: s32)
    {
        v := x
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