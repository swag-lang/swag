#global testerror "Err0099"
#test
{
    var x = 5
    let y =,constref &x
    y =,ref &x
}