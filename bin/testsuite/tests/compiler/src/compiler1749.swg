//#[Swag.PrintBc]
#test
{
    const array: [?] any = [true, 2, 3.0, "4"]
    #assert(cast(bool) array[0] == true)
    #assert(cast(string) array[3] == "4")
}
