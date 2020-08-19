#module "compiler1634"
#unittest error

struct A
{
    readonly 
    {
        x: u32    
        y: u32
    }
}

#run
{
    var st: A
    st.y = 0
}