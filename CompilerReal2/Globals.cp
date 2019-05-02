#import "Tree.cp"
#import "Type.cp"
#import "arrsSpecial.cp"
#import "BoxBlock.cp"
#import "PriorityRead.cp"
#import "BuiltInFuncs.cp"
#import "StandartObjects.cp"

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


DebugMode := false
cuId := int
DebugMetaData := Stack.{string}

UseDestructors := false

VoidPType := Type^
GTypeBool := Type^
GTypeDouble := Type^
GTypeFloat := Type^
GTypeInt := Type^
GTypeRange := Type^
GTypeVoid := Type^
GTypeVoidP := Type^
GTypeString := Type^
