#module "compiler1631"
#unittest error

struct A
{
    readonly x: u32    
}

#run
{
    var st: A
    st.x = 0
}