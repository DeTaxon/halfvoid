archSize := 64

TypeTable := Type^[30]
DefsTable := TypeDef^[30]

WorkBag := PriorityStack.{Object^,20}

PriorityData := PriorityBag^

ErrorLog := Queue.{string}

BuiltInFuncs := AVLMap.{string,Queue.{BoxFunc^}}
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

UseDestructors := false
UseFatString := false

getTempData :=  FuncInputBox^
workingOnObject := Object^

InCC := char^
OutCC := char^

VoidPType := Type^
GTypeBool := Type^
GTypeDouble := Type^
GTypeFloat := Type^
GTypeHalf := Type^
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
