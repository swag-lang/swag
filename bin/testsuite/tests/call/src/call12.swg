using tests_foreign

using Swag

//#[Swag.PrintBc, Swag.Optim(false)]
#test
{
    tcf_createThread(&tcf_entry, 666)
}