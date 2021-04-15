#global skip

// This is the list of predefined attributes
// All are located in the reserved 'swag' namespace

#[attributeUsage(AttributeUsage.Function)]
attr constexpr()

#[attributeUsage(AttributeUsage.Function|AttributeUsage.Struct|AttributeUsage.File)]
attr printbc()

#[attributeUsage(AttributeUsage.Function|AttributeUsage.GlobalVariable)]
attr compiler()

#[attributeUsage(AttributeUsage.Function)]
attr inline()

#[attributeUsage(AttributeUsage.Function)]
attr macro()

#[attributeUsage(AttributeUsage.Function)]
attr mixin()

#[attributeUsage(AttributeUsage.Function)]
attr test()

#[attributeUsage(AttributeUsage.Function)]
attr implicit()

// Hardcoded also for switch
#[attributeUsage(AttributeUsage.Function)]
attr complete()

#[attributeUsage(AttributeUsage.Function)]
attr noreturn()

#[attributeUsage(AttributeUsage.Function)]
attr foreign(module: string, function: string = "");

#[attributeUsage(AttributeUsage.Function)]
attr callback()

#[attributeUsage(AttributeUsage.Function|AttributeUsage.Variable)]
attr discardable()

#[attributeUsage(AttributeUsage.Function|AttributeUsage.Struct|AttributeUsage.Enum|AttributeUsage.EnumValue)]
attr deprecated(msg: string = null)

// Hardcoded for type alias
attr strict()

// Hardcoded for local variables
attr global()

// Hardcoded for struct and variables
attr align(value: u8)

#[attributeUsage(AttributeUsage.Struct)]
attr pack(value: u8)

#[attributeUsage(AttributeUsage.Struct)]
attr nocopy()

#[attributeUsage(AttributeUsage.StructVariable)]
attr offset(name: string)

#[attributeUsage(AttributeUsage.Enum)]
attr enumflags()

#[attributeUsage(AttributeUsage.Enum)]
attr enumindex()

#[attributeUsage(AttributeUsage.All)]
attr safety(what: string, value: bool)

#[attributeUsage(AttributeUsage.All)]
attr selectif(value: bool)

#[attributeUsage(AttributeUsage.Function|AttributeUsage.File)]
attr optim(what: string, value: bool)
