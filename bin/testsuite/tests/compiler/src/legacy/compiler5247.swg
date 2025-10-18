#global testerror "Err0017"
#test
{
    var x = 5
    let y = #constref &x
    y = #constref &x
}
