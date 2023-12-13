#global testerror "Err0564"
#test
{
    var x = 5
    let y = const ref &x
    y = ref &x
}