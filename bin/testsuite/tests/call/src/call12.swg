using test_foreign

using Swag

//#[Swag.PrintBc, Swag.Optimize(false)]
#test
{
    tcf_createThread(&tcf_entry, 666)
}