using tests_foreign

using Swag

//#[Swag.PrintBc, Swag.optim(false)]
#test
{
    tcf_join8("titi", 1, 2, 3, 4)
    tcf_join16("titi", 1, 2, 3, 4)
    tcf_join32("titi", 1, 2, 3, 4)
}