#global testerror "Err0020"
#test
{
    var x = 5
    let y = #constref &x
    y = #constref &x
}
