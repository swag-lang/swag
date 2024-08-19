#global testerror "Err0676"

readonly struct A
{
    x: u32
    y: u32
}

#run
{
    var st: A
    st.y = 0
}