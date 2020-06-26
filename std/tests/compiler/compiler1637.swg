readonly struct A
{
    x: u32    
    readwrite y: u32
}

#run
{
    var st: A
    st.y = 0
}