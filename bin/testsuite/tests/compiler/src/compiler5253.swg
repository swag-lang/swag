#global testerror "Err0646"
#test
{
    var x = 5
    var y = ref &x
    y = const ref &x
}