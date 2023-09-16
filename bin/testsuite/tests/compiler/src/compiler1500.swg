#global testerror "Err1172"

#test
{
    var toto[512]: u16
    @print(cast(s64) @sizeof(toto))
}