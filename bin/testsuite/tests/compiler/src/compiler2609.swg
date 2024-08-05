#global testerror "Err0391"

struct(T) AA
{

}

impl AA
{
    #assert @typeof(T) != s32
}

#test
{
    var x: AA's32
}