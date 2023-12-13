#global testerror "Err0177"
#test
{
    var x = 5
    var y = ref &x
    y = const ref &x
}