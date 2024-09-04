#global testerror "Err0468"

#test
{
    var toto[512]: u16
    @print(cast(s64) #sizeof(toto))
}