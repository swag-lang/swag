#global testerror "Err0094"
#test
{
    var x = 5
    let y = #ref &x
    y = #ref &x
}