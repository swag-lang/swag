
#global testerror "Err0585"

struct A
{
    readonly x: u32
}

#run
{
    var st: A
    st.x = 0
}