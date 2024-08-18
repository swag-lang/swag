#global testerror "Err0212"

#test
{
    loop i in 1'u64 to 0'u64 
    {       
        @print(@typeof(i).name) 
    }
}