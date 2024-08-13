
#global testerror "Err0476"

struct A
{
    readonly x: u32
}

#run
{
    var st: A
    st.x = 0
}