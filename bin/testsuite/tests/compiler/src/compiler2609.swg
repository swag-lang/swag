#global testerror "Err0252"

struct(T) AA
{

}

impl AA
{
    #assert(#typeof(T) != s32)
}

#test
{
    var x: AA's32
}