#global testerror "Err0112"
#test
{
    var x = 5
    let y = #constref &x
    y = #constref &x
}