#global testerror "Err0298"

#test
{
    loop i: 1'u64 to 0'u64 
    {       
        @print(@typeof(i).name) 
    }
}