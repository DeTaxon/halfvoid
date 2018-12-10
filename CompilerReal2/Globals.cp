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
LexMachine := Machine^

ErrorLog := Queue.{string}

BuiltInFuncs := Queue.{BoxFunc^}
BuiltInTemplates := Queue.{BoxTemplate^}

GlobalUnroll := BuiltInTemplateUnroll^
//GlobalNew := BuiltInTemplateNew^
GlobalUnpoint := BuiltInTemplatePoint^
GlobalRefExc := BuiltInTemplateRefEx^
GlobalExcArr := BuiltInTemplateExcArr^

VoidPType := Type^

GlobalStrs := string

Classes := Queue.{BoxClass^}

PostFuncs := Queue.{BuiltInFuncClassInfo^}

ForcedLibs := Queue.{BoxFile^}

DebugMode := false
cuId := int
DebugMetaData := Stack.{string}

UseDestructors := false
