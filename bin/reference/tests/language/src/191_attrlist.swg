#global skip

// This is the list of predefined attributes
// All are located in the reserved 'swag' namespace

#[AttrUsage(AttributeUsage.Function)]
attr ConstExpr()

#[AttrUsage(AttributeUsage.Function|AttributeUsage.Struct|AttributeUsage.File)]
attr PrintBc()

#[AttrUsage(AttributeUsage.Function|AttributeUsage.GlobalVariable)]
attr Compiler()

#[AttrUsage(AttributeUsage.Function)]
attr Inline()

#[AttrUsage(AttributeUsage.Function)]
attr Macro()

#[AttrUsage(AttributeUsage.Function)]
attr Mixin()

#[AttrUsage(AttributeUsage.Function)]
attr Test()

#[AttrUsage(AttributeUsage.Function)]
attr Implicit()

// Hardcoded also for switch
#[AttrUsage(AttributeUsage.Function)]
attr Complete()

#[AttrUsage(AttributeUsage.Function)]
attr NoReturn()

#[AttrUsage(AttributeUsage.Function)]
attr Foreign(module: string, function: string = "");

#[AttrUsage(AttributeUsage.Function)]
attr Callback()

#[AttrUsage(AttributeUsage.Function|AttributeUsage.Variable)]
attr Discardable()

#[AttrUsage(AttributeUsage.Function|AttributeUsage.Struct|AttributeUsage.Enum|AttributeUsage.EnumValue)]
attr Deprecated(msg: string = null)

// Hardcoded for type alias
attr Strict()

// Hardcoded for local variables
attr Global()

// Hardcoded for struct and variables
attr Align(value: u8)

#[AttrUsage(AttributeUsage.Struct)]
attr Pack(value: u8)

#[AttrUsage(AttributeUsage.Struct)]
attr NoCopy()

#[AttrUsage(AttributeUsage.StructVariable)]
attr Offset(name: string)

#[AttrUsage(AttributeUsage.Enum)]
attr EnumFlags()

#[AttrUsage(AttributeUsage.Enum)]
attr EnumIndex()

#[AttrUsage(AttributeUsage.All)]
attr Safety(what: string, value: bool)

#[AttrUsage(AttributeUsage.All)]
attr SelectIf(value: bool)

#[AttrUsage(AttributeUsage.Function|AttributeUsage.File)]
attr Optim(what: string, value: bool)
