#global skip

// This is the list of predefined attributes
// All are located in the reserved 'swag' namespace

#[AttrUsage(AttributeUsage.Function)]
attr ConstExpr()

#[AttrUsage(AttributeUsage.Function|AttributeUsage.Struct|AttributeUsage.File)]
attr PrintBc()

#[AttrUsage(AttributeUsage.Function|AttributeUsage.GlobalVariable)]
attr compiler()

#[AttrUsage(AttributeUsage.Function)]
attr inline()

#[AttrUsage(AttributeUsage.Function)]
attr macro()

#[AttrUsage(AttributeUsage.Function)]
attr mixin()

#[AttrUsage(AttributeUsage.Function)]
attr test()

#[AttrUsage(AttributeUsage.Function)]
attr implicit()

// Hardcoded also for switch
#[AttrUsage(AttributeUsage.Function)]
attr Complete()

#[AttrUsage(AttributeUsage.Function)]
attr NoReturn()

#[AttrUsage(AttributeUsage.Function)]
attr foreign(module: string, function: string = "");

#[AttrUsage(AttributeUsage.Function)]
attr callback()

#[AttrUsage(AttributeUsage.Function|AttributeUsage.Variable)]
attr Discardable()

#[AttrUsage(AttributeUsage.Function|AttributeUsage.Struct|AttributeUsage.Enum|AttributeUsage.EnumValue)]
attr Deprecated(msg: string = null)

// Hardcoded for type alias
attr Strict()

// Hardcoded for local variables
attr global()

// Hardcoded for struct and variables
attr align(value: u8)

#[AttrUsage(AttributeUsage.Struct)]
attr pack(value: u8)

#[AttrUsage(AttributeUsage.Struct)]
attr NoCopy()

#[AttrUsage(AttributeUsage.StructVariable)]
attr offset(name: string)

#[AttrUsage(AttributeUsage.Enum)]
attr EnumFlags()

#[AttrUsage(AttributeUsage.Enum)]
attr EnumIndex()

#[AttrUsage(AttributeUsage.All)]
attr Safety(what: string, value: bool)

#[AttrUsage(AttributeUsage.All)]
attr selectif(value: bool)

#[AttrUsage(AttributeUsage.Function|AttributeUsage.File)]
attr optim(what: string, value: bool)
