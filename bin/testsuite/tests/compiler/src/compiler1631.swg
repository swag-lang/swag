
#global testerror "Err0584"

struct A
{
    readonly x: u32
}

#run
{
    var st: A
    st.x = 0
}