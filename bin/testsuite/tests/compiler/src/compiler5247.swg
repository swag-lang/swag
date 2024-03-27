#global testerror "Err0104"
#test
{
    var x = 5
    let y = const ref &x
    y = const ref &x
}
