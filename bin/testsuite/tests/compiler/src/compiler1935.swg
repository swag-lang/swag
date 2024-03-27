struct SALUTE
{
    s, t: f64
}

//#[Swag.PrintBc]
func cc(st: *SALUTE)
{
    st.s = 55
    st.t = 55
}

//#[Swag.PrintBc]
#test
{
    var st: SALUTE
    cc(&st)
}