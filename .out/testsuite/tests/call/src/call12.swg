using tests_foreign

using Swag

//#[Swag.printbc, Swag.optim(false)]
#test
{
    tcf_createThread(&tcf_entry, 666)
}