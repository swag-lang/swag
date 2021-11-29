#global testerror
func toto()->{x: s32}
{
    return @{1}
}

#test
{
    (a, b) := toto()
}