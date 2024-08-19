#global testerror "Err0094"
#test
{
    var x = 5
    let y = #constref &x
    y = #constref &x
}