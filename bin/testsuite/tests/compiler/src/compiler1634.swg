
#global testerror "Err0582"

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