#global testerror "Err0396"

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