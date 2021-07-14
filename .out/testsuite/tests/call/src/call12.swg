using tests_foreign

using Swag

//#[Swag.PrintBc, Swag.optim(false)]
#test
{
    tcf_createThread(&tcf_entry, 666)
}