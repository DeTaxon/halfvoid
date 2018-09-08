
TypeTable := Type^[30]
DefsTable := TypeDef^[30]

WorkBag := PriorityStack.{Object^}

PriorityData := PriorityBag^
LexMachine := Machine^

ErrorLog := Queue.{string}

BuiltInFuncs := Queue.{BoxFunc^}
BuiltInTemplates := Queue.{BoxTemplate^}

GlobalUnroll := BuiltInTemplateUnroll^
GlobalNew := BuiltInTemplateNew^
GlobalUnpoint := BuiltInTemplatePoint^
GlobalRefExc := BuiltInTemplateRefEx^
GlobalExcArr := BuiltInTemplateExcArr^

VoidPType := Type^

GlobalStrs := string

Classes := Queue.{BoxClass^}

PostFuncs := Queue.{BuiltInFuncClassInfo^}

ForcedLibs := Queue.{BoxFile^}

DebugMode := false
DebugMetaData := Stack.{string}
