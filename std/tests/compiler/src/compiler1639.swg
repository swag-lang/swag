
#unittest error

internal struct A
{
    x: u32
    y: u32
}

#run
{
    var st: A
    if st.y == 0
        @assert(false)
}