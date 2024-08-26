#global testerror "Err0112"
#test
{
    var x = 5
    let y = #ref &x
    y = #ref &x
}