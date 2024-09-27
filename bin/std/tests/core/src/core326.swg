using Core, Core.Time

struct Employee
{
    name:     String
    date:     Time.DateTime
}

#[Swag.Compiler, Swag.ConstExpr]
func gen(type: nn const *Swag.TypeInfoStruct)->String
{
    var result: String = "return "

    foreach f in type.fields
    {
        if #index:
            result += " and "
        result += f.name
        result += " == "
        result += "other."
        result += f.name
    }

    return result
}

impl Employee
{
    mtd opEquals(other: Self)->bool
    {
        #ast
        {
            let res = gen(Employee)
            return cast(string) res
        }
    }
}

func years_employed(employee: Employee)->u32 => DateTime.now().year - employee.date.year

#run
{
    var alice  = Employee{String.from("Alice"), Time.DateTime{2000, 1, 1}}
    var alice2 = alice
    alice2.date = Time.DateTime{2010, 1, 1}
    var alice3 = alice2

    @assert(alice.years_employed() >= 22)
    @assert(alice != alice2)
    @assert(alice2 == alice3)
}
