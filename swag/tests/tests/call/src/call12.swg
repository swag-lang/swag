#import "tests_foreign"
using tests_foreign

using swag

//#[swag.printbc, swag.optim(false)]
#test
{
    tcf_createThread(&tcf_entry, 666)
}