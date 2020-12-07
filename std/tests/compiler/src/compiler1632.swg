#module "compiler1632"
#unittest error

struct A
{
    readonly x, y: u32    
}

#run
{
    var st: A
    st.y = 0
}