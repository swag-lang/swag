#global testerror "Err0538"

#test
{
    var toto[512]: u16
    @print(cast(s64) @sizeof(toto))
}