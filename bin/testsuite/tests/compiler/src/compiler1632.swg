
#global testerror "Err0536"

struct A
{
    readonly x, y: u32
}

#run
{
    var st: A
    st.y = 0
}