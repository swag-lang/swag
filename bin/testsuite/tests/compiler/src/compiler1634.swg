
#global testerror "Syn0070"

struct A
{
    readonly
    {
        x: u32
        y: u32
    }
}

#run
{
    var st: A
    st.y = 0
}