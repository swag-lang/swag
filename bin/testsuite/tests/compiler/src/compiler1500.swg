#global testerror "Err0578"

#test
{
    var toto[512]: u16
    @print(cast(s64) @sizeof(toto))
}