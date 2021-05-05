archSize := 64

TypeTable := Type^[30]
DefsTable := TypeDef^[30]

PriorityData := PriorityBag^

ErrorLog := Queue.{string}

BuiltInFuncs := HashAVLMap.{string,Queue.{BoxFunc^}}
BuiltInExcs := Queue.{BoxFunc^}
BuiltInTemplates := Queue.{BoxTemplate^}

GlobalUnroll := BuiltInTemplateUnroll^
//GlobalNew := BuiltInTemplateNew^
GlobalUnpoint := BuiltInTemplatePoint^
GlobalRefExc := BuiltInTemplateRefEx^
GlobalExcArr := BuiltInTemplateExcArr^

ReturnName := string

OpersTree := AVLSet.{string}

GlobalStrs := Queue.{string}

Classes := Queue.{BoxClass^}

PostFuncs := Queue.{BuiltInFuncClassInfo^}

CTT := CreateTupleTemplate^
TCT := TupleConstructorTemplate^

GlobalAttributes := AVLMap.{char^,Object^}
GlobalDataBuiltins := AVLMap.{char^,Object^}

UseDestructors := false

getTempData :=  FuncInputBox^
workingOnObject := Object^

InCC := char^
OutCC := char^

VoidPType := Type^
GTypeBool := Type^
GTypeDouble := Type^
GTypeFloat := Type^
GTypeHalf := Type^
GTypeU8 := Type^
GTypeChar := Type^
GTypeInt := Type^
GTypeU64 := Type^
GTypeRange := Type^
GTypeVoid := Type^
GTypeVoidP := Type^
GTypeString := Type^
GTypeVec4f := Type^
GTypeQuantf := Type^
GTypeVec3f := Type^
GTypeVec2f := Type^

GBoolTrue := Object^
GBoolFalse := Object^

EnableGSTask := bool

MacroRestart := Object^
