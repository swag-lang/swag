#global testerror "Err0564"
#test
{
    var x = 5
    let y = ref &x
    y = ref &x
}