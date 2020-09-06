#module "compiler1954"
#unittest error

#test
{
    const a = @{1, 2, 3, 4}
    visit v: a
    {
    }
}