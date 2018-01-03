# -*- coding: UTF-8 -*-
from Lex import *
from MathS import *
from allnums import *
from Type import *
from ParamChecks import *
import struct

ContextStuff = []
StuffStack = []

ContextName = []
NameStack = []

ConstTable = {}

StandartStuff = []

InFunc = []

Opers = ["+","-","*","/","%"]
CmprOpers = ["==",">=","<=","!=","<",">"]
PrisvOpers = ["=","+=","-=","*=","/=","%=","<<",">>"]
AllOpers = Opers + CmprOpers + PrisvOpers



def PushC():
	StuffStack.append(len(ContextStuff))
	NameStack.append(len(ContextName))
def PopC():
	Unt = StuffStack.pop()
	while len(ContextStuff) > Unt:
	    ContextStuff.pop()
	Unt = NameStack.pop()
	while len(ContextName) > Unt:
	    ContextName.pop()

def GetFunc(Name,Pars, SearchThis = False):
	PreRet = SearchFunc(Name,Pars,ContextStuff, SearchThis)
	if PreRet != None:
		return PreRet
	return SearchFunc(Name,Pars,StandartStuff)

def SearchFunc(Name,Pars,Arr, SearchThis = False):
    WrongValue = False
    for i in reversed(range(len(Arr))):
	Itc = Arr[i]
        if Itc.Name == Name:
	    if Itc.Value == "~:=":
            	It = Itc.Extra
		if Itc.Extra == None:
			continue
	    else:
		It = Itc
	    if not hasattr(It,"Params"):
	    	return None
	    if len(It.Params) > 0 and  (SearchThis != (It.Params[0].Name == "this")):
		continue
            WrongValue = False
	    if len(Pars) != len(It.Params) and It.Vargs == False: #Name != "printf":
		continue
	    if Name in ["=","+=","-=","/=","*=","%="]:
		if PrisvCheck(Pars,It.Params):
			return It
	    else:
            	if NormalCheck(Pars,It.Params, Name in AllOpers):
                	return It
    return None
def SearchFuncClose(Name,Arr, SearchThis = False):
    WrongValue = False
    for i in reversed(range(len(Arr))):
	Itc = Arr[i]
        if Itc.Name == Name:
		return Itc
    return None

def GetParam(Name, GetFPoint = False):
    WrongValue = False
    for i in reversed(range(len(ContextStuff))):#ContextStuff:
	It = ContextStuff[i]
	if It.Name != Name:
		continue
	if GetFPoint  and(It.Type == None or (It.Type.Type != "funcp") or (It.IsRef != False)):
		continue
        return It
    if Name in ConstTable:	
	return ConstTable[Name]
    return None


class BoxFor:
    def __init__(self,Obj):
        self.Id = GetNumb()
        self.Value = "~for"
	self.Job = None
	self.Quest = None #How?
	self.ParName = "it"
	self.IndName = "~hiden"
	self.Id = GetNumb()
	self.IsNumI = False
	self.IsRange = False 
	self.ParParam = None
	self.IndParam = None
	self.IsFixed = -1
	self.FuncFor = None
	self.IsString = False
	JobPos = -1
        if len(Obj.Extra) == 3:
		JobPos = 2
        if len(Obj.Extra) == 5:
		JobPos = 4
		self.ParName = Obj.Extra[1].Extra
        if len(Obj.Extra) == 7:
		JobPos = 6
		self.ParName = Obj.Extra[1].Extra
		self.IndName = Obj.Extra[3].Extra

	self.Quest = GetUse(Obj.Extra[JobPos - 1])
        if Obj.Extra[JobPos].Value == "{}":
            self.Job = BoxBlock(Obj.Extra[JobPos].Extra)
        else:
            self.Job = GetUse(Obj.Extra[JobPos])
		
		
    def PrintInBlock(self,F):
	if self.IsRange:
		self.ParParam.PrintPointPre(F)
		F.write("store i32 {}, i32* {}\n".format(self.Quest.L.Extra,self.ParParam.GetPName()))
        	F.write("br label %ForCheck{}\n".format(self.Id))
        	F.write("ForCheck{}:\n".format(self.Id))
		self.ParParam.PrintPre(F)
		F.write("%Result{} = icmp sle i32 {} , {}\n".format(self.Id,self.ParParam.GetName(),self.Quest.R.Extra))
        	F.write("br i1 %Result{0}, label %ForStart{0}, label %ForEnd{0}\n".format(self.Id))

        	F.write("ForStart{}:\n".format(self.Id))
        	self.Job.PrintInBlock(F)

		self.ParParam.PrintPre(F)
		F.write("%ToStore{} = add i32 {} , 1\n".format(self.Id,self.ParParam.GetName()))
		self.ParParam.PrintPointPre(F)
		F.write("store i32 %ToStore{},".format(self.Id))
		self.ParParam.PrintPointUse(F)
		F.write("\n")
        	F.write("br label %ForCheck{}\n".format(self.Id))
        	F.write("ForEnd{}:\n".format(self.Id))
	elif self.IsString:
		F.write("%TmpPoint{} = alloca i8*\n".format(self.Id))
		self.ParParam.PrintPointPre(F)
        	self.Quest.PrintPre(F)
		F.write("store i8* {}, i8** %TmpPoint{}\n".format(self.Quest.GetName(),self.Id))
		F.write("%TmpCurValue1{} = load i8 , i8* {}\n".format(self.Id , self.Quest.GetName()))
		F.write("store i8 %TmpCurValue1{}, i8* {}\n".format(self.Id,self.ParParam.GetPName()))
        	F.write("br label %ForCheck{}\n".format(self.Id))
        	F.write("ForCheck{}:\n".format(self.Id))
		self.ParParam.PrintPre(F)
		F.write("%Result{} = icmp ne i8 {} , 0\n".format(self.Id,self.ParParam.GetName()))
        	F.write("br i1 %Result{0}, label %ForStart{0}, label %ForEnd{0}\n".format(self.Id))

        	F.write("ForStart{}:\n".format(self.Id))
        	self.Job.PrintInBlock(F)

		F.write("%TmpValue21{0} = load i8* , i8** %TmpPoint{0}\n".format(self.Id))
		F.write("%TmpNext{0} = getelementptr i8 ,i8* %TmpValue21{0}, i32 1\n".format(self.Id))
		F.write("store i8* %TmpNext{0}, i8** %TmpPoint{0}\n".format(self.Id))
		
		F.write("%TmpValue2{0} = load i8 , i8* %TmpNext{0}\n".format(self.Id))
		self.ParParam.PrintPointPre(F)
		F.write("store i8 %TmpValue2{0}, i8* {1}\n".format(self.Id,self.ParParam.GetPName()))
		
        	F.write("br label %ForCheck{}\n".format(self.Id))
        	F.write("ForEnd{}:\n".format(self.Id))
	elif self.IsNumI:
        	self.Quest.PrintPre(F)
		F.write("%EndValue{} = add i32 {} , 0\n".format(self.Id,self.Quest.GetName()))
		self.ParParam.PrintPointPre(F)
		F.write("store i32 0, i32* {}\n".format(self.ParParam.GetPName()))
        	F.write("br label %ForCheck{}\n".format(self.Id))
        	F.write("ForCheck{}:\n".format(self.Id))
		self.ParParam.PrintPre(F)
		F.write("%Result{0} = icmp slt i32 {1} , %EndValue{0}\n".format(self.Id,self.ParParam.GetName()))
        	F.write("br i1 %Result{0}, label %ForStart{0}, label %ForEnd{0}\n".format(self.Id))

        	F.write("ForStart{}:\n".format(self.Id))
        	self.Job.PrintInBlock(F)

		self.ParParam.PrintPre(F)
		F.write("%ToStore{} = add i32 {} , 1\n".format(self.Id,self.ParParam.GetName()))
		self.ParParam.PrintPointPre(F)
		F.write("store i32 %ToStore{},".format(self.Id))
		self.ParParam.PrintPointUse(F)
		F.write("\n")
        	F.write("br label %ForCheck{}\n".format(self.Id))
        	F.write("ForEnd{}:\n".format(self.Id))
	elif self.IsFixed != -1:
		self.IndParam.PrintPointPre(F)
		F.write("store i32 0, i32* {}\n".format(self.IndParam.GetPName()))
        	F.write("br label %ForCheck{}\n".format(self.Id))
        	F.write("ForCheck{}:\n".format(self.Id))
        	self.Quest.PrintPointPre(F)
		self.IndParam.PrintPre(F)
		F.write("%Tmp{} = getelementptr ".format(self.ParParam.Id))
		self.ParParam.Type.PrintUse(F)
		F.write(", ")
		self.Quest.PrintPointUse(F)
		F.write(", ")
		self.IndParam.PrintUse(F)
		F.write("\n")
		F.write("%Result{} = icmp slt i32 {} , {}\n".format(self.Id,self.IndParam.GetName(),self.IsFixed))
        	F.write("br i1 %Result{0}, label %ForStart{0}, label %ForEnd{0}\n".format(self.Id))

        	F.write("ForStart{}:\n".format(self.Id))
        	self.Job.PrintInBlock(F)

		self.IndParam.PrintPre(F)
		F.write("%ToStore{} = add i32 {} , 1\n".format(self.Id,self.IndParam.GetName()))
		self.IndParam.PrintPointPre(F)
		F.write("store i32 %ToStore{},".format(self.Id))
		self.IndParam.PrintPointUse(F)
		F.write("\n")
        	F.write("br label %ForCheck{}\n".format(self.Id))
        	F.write("ForEnd{}:\n".format(self.Id))
	elif self.FuncFor != None:
		return None
    def PrintFunc(self,F):
	if self.Quest != None:
        	self.Quest.PrintFunc(F)
        self.Job.PrintFunc(F)
    def PrintAlloc(self,F):
        self.Job.PrintAlloc(F)
	if self.ParParam != None:
		self.ParParam.PrintAlloc(F)
	if self.IndParam != None:
		self.IndParam.PrintAlloc(F)
    def PrintConst(self,F):
	if self.Quest != None:
        	self.Quest.PrintConst(F)
        self.Job.PrintConst(F)
    def Check(self):
        self.Quest.Check()
	if self.Quest.Value == "~d..d":
		self.IsRange = True
		self.ParParam = ParamChain(GetType("int"),Token("id",self.ParName))
		#self.IndParam = ParamChain(GetType("int"),Token("id",self.IndName))
	elif self.Quest.Type.Id == GetType("int").Id:
		self.IsNumI = True
		self.ParParam = ParamChain(GetType("int"),Token("id",self.ParName))
	elif self.Quest.Type.Id == GetPoint(GetType("char")).Id:
		self.IsString = True
		self.ParParam = ParamChain(GetType("char"),Token("id",self.ParName))
	elif self.Quest.Type.Type == "fixed":
		self.IsFixed = self.Quest.Type.Size
		self.ParParam = ParamChain(self.Quest.Type.Base,Token("id",self.ParName))
		self.ParParam.IsDrived = True
		self.IndParam = ParamChain(GetType("int"),Token("id",self.IndName))
	elif self.Quest.Type.Type == "class":
		self.FuncFor = self.Quest.Type.GetFunc("for",[self.Quest])
		if self.FuncFor == None:
			raise ValueError("Cant build for")
	else:
		raise ValueError("Does not work with for")
	PushC()
	if self.ParParam != None:
		ContextStuff.append(self.ParParam)
	if self.IndParam != None:
		ContextStuff.append(self.IndParam)
        self.Job.Check()
	PopC()
class BoxWhile:
    def __init__(self,Obj):
        self.Id = GetNumb()
        self.Value = "~while"
        self.IsPost = (Obj.Extra[0].Value == "do")

        if self.IsPost:
            self.Quest = GetUse(Obj.Extra[3])
            if Obj.Extra[1].Value == "{}":
                self.Job = BoxBlock(Obj.Extra[1].Extra)
            else:
                self.Job = GetUse(Obj.Extra[1])
        else:
            self.Quest = GetUse(Obj.Extra[1])
            if Obj.Extra[2].Value == "{}":
                self.Job = BoxBlock(Obj.Extra[2].Extra)
            else:
                self.Job = GetUse(Obj.Extra[2])
    def PrintInBlock(self,F):
        if self.IsPost:
            F.write("br label %WhileT{}\n".format(self.Id))
        else:
            F.write("br label %WhileC{}\n".format(self.Id))
        F.write("WhileC{}:\n".format(self.Id))
        self.Quest.PrintPre(F)
        F.write("br ")
        self.Quest.PrintUse(F)
        F.write(", label %WhileT{0}, label %WhileF{0}\n".format(self.Id))

        F.write("WhileT{}:\n".format(self.Id))
        self.Job.PrintInBlock(F)
        F.write("br label %WhileC{}\n".format(self.Id))
        F.write("WhileF{}:\n".format(self.Id))
    def PrintFunc(self,F):
        self.Quest.PrintFunc(F)
        self.Job.PrintFunc(F)
    def PrintAlloc(self,F):
        self.Job.PrintAlloc(F)
    def PrintConst(self,F):
        self.Quest.PrintConst(F)
        self.Job.PrintConst(F)
    def Check(self):
        self.Quest.Check()
        self.Job.Check()
        if self.Quest.Type != GetType("bool").Id:
            self.Quest = BoxExc(self.Quest,GetType("bool"))
class BoxIf:
    def __init__(self,Obj):
        self.Id = GetNumb()
        self.Value = "~if"
        self.Quest = GetUse(Obj.Extra[1])
        if Obj.Extra[2].Value == "{}":
            self.OnTrue = BoxBlock(Obj.Extra[2].Extra)
        else:
            self.OnTrue = GetUse(Obj.Extra[2])
        self.OnFalse = None
        if len(Obj.Extra) > 3:
		if Obj.Extra[4].Value == "{}":
			self.OnFalse = BoxBlock(Obj.Extra[4].Extra)
		else:
			self.OnFalse = GetUse(Obj.Extra[4])
    def PrintInBlock(self,F):
        self.Quest.PrintPre(F)
        F.write("br ")
        self.Quest.PrintUse(F)
        F.write(", label %IfTrue{}".format(self.Id))
        if self.OnFalse != None:
            F.write(", label %IfFalse{}\n".format(self.Id))
        else:
            F.write(", label %IfEnd{}\n".format(self.Id))
        F.write("IfTrue{}:\n".format(self.Id))
        self.OnTrue.PrintInBlock(F)
        F.write("br label %IfEnd{}\n".format(self.Id))
        if self.OnFalse != None:
            F.write("IfFalse{}:\n".format(self.Id))
            self.OnFalse.PrintInBlock(F)
            F.write("br label %IfEnd{}\n".format(self.Id))
        F.write("IfEnd{}:\n".format(self.Id))
    def PrintFunc(self,F):
        self.Quest.PrintFunc(F)
        self.OnTrue.PrintFunc(F)
        if self.OnFalse != None:
            self.OnFalse.PrintFunc(F)
    def PrintAlloc(self,F):
        self.OnTrue.PrintAlloc(F)
        if self.OnFalse != None:
            self.OnFalse.PrintAlloc(F)
    def PrintConst(self,F):
        self.Quest.PrintConst(F)
        self.OnTrue.PrintConst(F)
        if self.OnFalse != None:
            self.OnFalse.PrintConst(F)
    def Check(self):
        self.Quest.Check()
        self.OnTrue.Check()
        if self.OnFalse != None:
            self.OnFalse.Check()
        if self.Quest.Type.Id != GetType("bool").Id:
            self.Quest = BoxExc(self.Quest,GetType("bool"))
class ParArrConst:
	def __init__(self,Obj):
		self.Value = "~![]"
		self.Id = GetNumb()
		self.Extra = []

		for It in Obj.Extra[1].Extra:
			if It.Value != ',':
				if It.Value == "d..d":
					DdD = GetUse(It)
					for R in range(DdD.L.Extra,DdD.R.Extra + 1):
						self.Extra.append(GetUse(Token("numi",R)))
				else:
					self.Extra.append(GetUse(It))
		if len(self.Extra) == 0:
			raise ValueError("Empty array")
		self.Type = GetFixedArr(self.Extra[0].Type, len(self.Extra))
	def PrintPointUse(self,F):
		F.write("i32* %Tmp{}".format(self.PrevId))
	def PrintUse(self,F):
		F.write("i32* %Tmp{}".format(self.PrevId))
	def GetName(self):
		return "{}".format(self.Extra)
	def PrintPointPre(self,F):
		self.PrevId = GetNumb()
		F.write("%Tmp{0} = getelementptr [ {1} x i32 ] , [ {1} x i32 ]* @Tmp{2}, i32 0,i32 0\n".format(self.PrevId,len(self.Extra),self.Id))
		return None
	def PrintPre(self,F):
		self.PrevId = GetNumb()
		F.write("%Tmp{0} = getelementptr [ {1} x i32 ] , [ {1} x i32 ]* @Tmp{2}, i32 0,i32 0\n".format(self.PrevId,len(self.Extra),self.Id))
		return None
	def PrintFunc(self,F):
		return None
	def PrintConst(self,F):
		F.write("@Tmp{} = global [{} x i32] [".format(self.Id,len(self.Extra)))
		for i in range(len(self.Extra)):
			if i != 0:
				F.write(",")
			self.Extra[i].PrintAsConst(F)
		F.write("]\n")
		return None
	def PrintAlloc(self,F):
		return None
	def Check(self):
		return None

class BoxConst:
	def __init__(self,Obj):
		self.Value = "~const"
		self.Name = Obj.Extra[0].Extra
		self.Type = None
		self.Id = -1
		if Obj.Extra[2].Value == "numi":
			self.Extra = Obj.Extra[2].Extra
			self.Type = GetType("int")
	def PrintUse(self,F):
		F.write("i32 {}".format(self.Extra))
	def GetName(self):
		return "{}".format(self.Extra)
	def PrintPre(self,F):
		return None
	def PrintFunc(self,F):
		return None
	def PrintConst(self,F):
		return None
	def PrintAlloc(self,F):
		return None
	def Check(self):
		return None

        
class ParamChain:
    def __init__(self,Obj, NotObj = None):
        self.Value = "~:="
        self.IsFunc = False
        self.Id = GetNumb()
        self.IsRef = True
	self.IsDrived = False
	self.IsGlobal = False
	self.PreExtra = None
	self.IsChecked = False
	self.IsFuncPointer = False
	self.Extra = None
	self.HaveConstr = False
	self.Params = []
	self.ToCall = None
	self.Type = None
	self.ItsExtern = False

        if NotObj == None:
            self.Name = Obj.Extra[0].Extra
            self.Extra = None
            self.Type = None

            if Obj.Extra[2].Value == "!()":
                self.IsFunc = True
		self.IsRef = False
                self.Extra = BoxFunc(Obj.Extra[2])
                self.Extra.Name = self.Name
            elif Obj.Extra[2].Value == "extern":
		self.ItsExtern = True
		self.PreExtra = Obj.Extra[3]
	    else:
		self.PreExtra = Obj.Extra[2]
        else:
            self.Name = None
            if NotObj != "~no":
                self.Name = NotObj.Extra
	    else:
		self.IsRef = False
            self.Extra = None
            self.Type =  Obj
    def PrintInFunc(self,F):
	if self.IsRef:
		GetPoint(self.Type).PrintUse(F)
	else:
		self.Type.PrintUse(F)
	F.write(" %Tmp{}".format(self.Id))
    def PrintParam(self,F):
        if self.Type != None:
		if self.IsRef:
			GetPoint(self.Type).PrintUse(F)
		else:
            		self.Type.PrintUse(F)
        F.write(" ")
        if self.Name != None:
            F.write("%Tmp{}".format(self.Id))
            #F.write(self.Name)
    def PrintConst(self,F):
        if self.Extra != None:
            self.Extra.PrintConst(F)
	for It in self.Params:
		It.PrintConst(F)
        return None
    def PrintPre(self,F):
        if self.IsFunc:
            return None
	if self.Type.Type == "fixed":
		return self.PrintPointPre(F)
        if self.IsRef:
            self.PrevId = GetNumb()
            F.write("%Tmp{} = load ".format(self.PrevId))
            self.Type.PrintUse(F)
            F.write(" , ")
	    if self.Type.Type == "fixed":
            	GetPoint(self.Type).PrintUse(F)
	    else:
            	self.Type.PrintUse(F)
	    if self.IsGlobal:
            	F.write("* @Tmp{}\n".format(self.Id))
	    else:
            	F.write("* %Tmp{}\n".format(self.Id))
    def PrintUse(self,F):
        if self.IsFunc:
            self.Extra.PrintFPoint(F)
            F.write(" @{}".format(self.Extra.FullName))
            return None
	if self.Type.Type == "fixed":
		return self.PrintPointUse(F)
        if self.IsRef:
            self.Type.PrintUse(F)
            F.write(" %Tmp{}".format(self.PrevId))
        else:
            self.Type.PrintUse(F)
            F.write(" %Tmp{}".format(self.Id))
    def PrintFunc(self,F):
        if not self.IsFunc:
            return None
        self.Extra.PrintFunc(F)
    def Check(self):
	if self.IsChecked:
		return None
	if self.IsGlobal:
		self.IsRef = True
	self.IsChecked = True
	if self.PreExtra != None and self.PreExtra.Value == "d()" and self.PreExtra.Extra[0].Value in ["id","d.{}"]:
		NType = ParseType(self.PreExtra.Extra[0])
		if NType != None and NType.Type == "class":
			self.Type = NType
			for Items in self.PreExtra.Extra[1].Extra:
				if Items.Value != ",":
					self.Params.append(GetUse(Items))
			for Items in self.Params:
				Items.Check()
			self.HaveConstr = True
			self.ToCall = self.Type.GetFunc("this",[self] + self.Params)
			if self.ToCall == None:
				raise ValueError("Constructor not found")
			self.IsChecked = True
			return None
	if self.PreExtra != None:
		self.Type = ParseType(self.PreExtra)
		if self.Type == None:
			self.Extra = GetUse(self.PreExtra)
			if self.Extra == None:
				raise ValueError("Unknown Object")
        if self.Extra != None and not self.HaveConstr:
            self.Extra.Check()
            self.Type = self.Extra.Type
	    if self.Type.Type == "class":
		self.ToCall = self.Type.GetFunc("=",[self] + [self.Extra])
	if self.IsFunc:
		self.Type = self.Extra.GetFuncPointType()
	if self.Type.Type == "class":
		self.IsRef = True
    def GetPName(self):
	if self.ItsExtern:
		return "@{}".format(self.Name)
	if self.IsGlobal:
		return "@Tmp{}".format(self.Id)
	else:
		return "%Tmp{}".format(self.Id)
    def GetName(self):
	if self.Type.Type == "fixed":
		return self.GetPName()
	if self.IsGlobal:
		if self.IsRef:
			return "%Tmp{}".format(self.PrevId)
		else:
        		return "@Tmp{}".format(self.Id)
	else:
		if self.IsRef:
        		return "%Tmp{}".format(self.PrevId)
		else:
			return "%Tmp{}".format(self.Id)
    def PrintAlloc(self,F):
	if self.Extra != None and not self.HaveConstr:
		self.Extra.PrintAlloc(F)
	if self.IsGlobal:
		raise ValueError("Impossible error")
	if self.IsDrived:
		return None
	if not self.IsFunc:
		if self.Type.Type == "fixed" and False: #TODO: проверить
            		F.write("%Tmp{}Pre = alloca ".format(self.Id))
            		self.Type.PrintInAlloc(F)
            		F.write("\n")
            		F.write("%Tmp{} = bitcast ".format(self.Id))
            		self.Type.PrintInAlloc(F)
			F.write("* %Tmp{}Pre to ".format(self.Id))
            		GetPoint(self.Type).PrintUse(F)
            		F.write("\n")
		else:
            		F.write("%Tmp{} = alloca ".format(self.Id))
            		self.Type.PrintInAlloc(F)
            		F.write("\n")
	return None
    def PrintPointPre(self,F):
	if not self.IsRef:
		raise ValueError("Not a refence")
	if self.Type.Type == "fixed":
		self.PrevId = GetNumb()
		F.write("%Tmp{} = bitcast ".format(self.PrevId))
		self.Type.PrintInAlloc(F)
		if self.IsGlobal:
			F.write("* @Tmp{}".format(self.Id))	
		else:
			F.write("* %Tmp{}".format(self.Id))
		F.write(" to ")
		self.Type.PrintUse(F)
		F.write("\n")
    def PrintPointUse(self,F):
	if self.ItsExtern:
		GetPoint(self.Type).PrintUse(F)
		F.write(" @{}".format(self.Name))
		return None
	if self.Type.Type == "fixed":
		if self.IsGlobal:
			GetPoint(self.Type).PrintUse(F)
			F.write(" %Tmp{}".format(self.PrevId))
		else:
			GetPoint(self.Type).PrintUse(F)
			F.write(" %Tmp{}".format(self.PrevId))
	else:
		GetPoint(self.Type).PrintUse(F)
		if self.IsGlobal:
			F.write(" @Tmp{}".format(self.Id))
		else:
			F.write(" %Tmp{}".format(self.Id))
    def PrintInBlock(self,F):
	if self.IsGlobal and not self.IsFunc:
		if self.Extra != None:
			if self.Extra.Value == "~num":
				F.write("@Tmp{} = global ".format(self.Id))
				self.Extra.PrintAsConst(F)
				F.write("\n")
			elif self.Extra.Value == "~str":
				F.write("@Tmp{0} = global i8* getelementptr ([{1} x i8], [{1} x i8]* @CStr{2},i32 0, i32 0),align 8\n".format(self.Id,self.Extra.Size,self.Extra.Id))
		else:
			if self.ItsExtern:
				F.write("@{} = external global ".format(self.Name))
				self.Type.PrintUse(F)
				F.write("\n")
			else:
				F.write("@Tmp{} = global ".format(self.Id))
				self.Type.PrintZero(F)
				F.write("\n")
		return None
        if not self.IsFunc and self.Type != None:
            if self.Extra != None or self.HaveConstr:
		if self.HaveConstr:
			self.ToCall.PrintUse(F,-1,[self] + self.Params)
		elif self.ToCall != None:
			self.ToCall.PrintUse(F,-1,[self] + [self.Extra])
		else:
                	self.Extra.PrintPre(F)
                	F.write("store ")
                	self.Extra.PrintUse(F)
                	F.write(", ")
                	self.Type.PrintUse(F)
                	F.write("* %Tmp{}\n".format(self.Id))

	

class BoxNew:
	def __init__(self,Obj):
		self.Value = "~new"
		self.Type = None
		self.Extra = None
		self.PreType = None
		self.HaveConstr = False
		self.Size = None
		self.Pars = []
		self.ToUse = None
		self.AddedThis = False
		self.Line = Obj.Line
		self.InFile = Obj.InFile
		
		U = Obj.Extra[1]
		if U.Value == "d()":
			self.PreType = U.Extra[0]
			U = U.Extra[1]
			self.HaveConstr = True
			for it in U.Extra:
				if it.Value != ',':
					self.Pars.append(GetUse(it))
		else:
			if U.Value == "d[]":
				self.PreType = U.Extra[0]
				self.Size = GetUse(U.Extra[1].Extra[0])
			else:
				self.PreType = U
	def PrintConst(self,F):
		if self.Size != None:
			self.Size.PrintConst(F)
		if self.Extra != None:
			self.Extra.PrintConst(F)
		for it in self.Pars:
			it.PrintConst(F)
	def PrintFunc(self,F):
		if self.Size != None:
			self.Size.PrintFunc(F)
		if self.Extra != None:
			self.Extra.PrintFunc(F)
	def PrintAlloc(self,F):
		if self.Size != None:
			self.Size.PrintAlloc(F)
		if self.Extra != None:
			self.Extra.PrintAlloc(F)
	def PrintPre(self,F):
		self.PrevId = GetNumb()
		F.write("%EndPos{} = getelementptr ".format(self.PrevId))
		self.Type.Base.PrintUse(F)
		F.write(" , ")
		self.Type.PrintUse(F)
		F.write(" null,i32 1\n")
		F.write("%Size{} = ptrtoint ".format(self.PrevId))
		self.Type.PrintUse(F)
		F.write(" %EndPos{} to i32\n".format(self.PrevId))
		if self.Size == None:
			F.write("%EndSize{0} = mul i32 1,%Size{0}\n".format(self.PrevId))
		else:
			self.Size.PrintPre(F)
			F.write("%EndSize{0} = mul i32 {1},%Size{0}\n".format(self.PrevId,self.Size.GetName()))
		F.write("%TmpPre{0} = call i8* @calloc(i32 1,i32 %EndSize{0})\n".format(self.PrevId))
		F.write("%Tmp{0} = bitcast i8* %TmpPre{0} to ".format(self.PrevId))
		self.Type.PrintUse(F)
		F.write("\n")
		if self.Type.Base.Type == "class" and self.Type.Base.VirtualFuncs != None:
			F.write("%ZeroPos{0} = getelementptr ".format(self.PrevId))
			self.Type.Base.PrintInAlloc(F)
			F.write(" , ")
			self.Type.PrintInAlloc(F)
			F.write(" %Tmp{0}, i32 0,i32 0\n".format(self.PrevId))
			F.write("store ")
			self.Type.Base.VirtualFuncs.PrintUse(F)
			F.write(", ")
			self.Type.Base.VirtualFuncs.PrintType(F)
			F.write("* %ZeroPos{}\n".format(self.PrevId))
		if self.HaveConstr:
			for i in range(1,len(self.Pars)):
				if self.ToUse.Params[i].IsRef:
					self.Pars[i].PrintPointPre(F)
				else:
					self.Pars[i].PrintPre(F)
				
			F.write("call void @{}(".format(self.ToUse.FullName))
			self.Type.PrintUse(F)
			F.write(" %Tmp{}".format(self.PrevId))
			for i in range(1,len(self.Pars)):
				F.write(" , ")
				if self.ToUse.Params[i].IsRef:
					self.Pars[i].PrintPointUse(F)
				else:
					self.Pars[i].PrintUse(F)
			F.write(")\n")

	def PrintUse(self,F):
		self.Type.PrintUse(F)
		F.write(" %Tmp{0}".format(self.PrevId))	
	def GetName(self):
		return " %Tmp{0}".format(self.PrevId)
	def Check(self):
		if self.Extra != None:
			self.Extra.Check()
		for it in self.Pars:
			it.Check()
		self.Type = ParseType(self.PreType)
		if self.Type == None:
			raise ValueError("Not found type at line {} in file {}".format(self.Line,self.InFile))
		if self.Size != None:
			self.Size.Check()
			if self.Size.Type.Id != GetType("int").Id:
				raise ValueError("Must be int")
		if self.HaveConstr:
			if not self.AddedThis:
				self.Pars = [ParamChain(self.Type,Token("id","this"))] + self.Pars
				self.AddedThis = True
			if self.Type.Type != "class":
				raise ValueError("Not a class")
			self.Type.Check()
			self.ToUse = self.Type.GetFunc("this",self.Pars)
			if self.ToUse == None:
				raise ValueError("Constructor not found at {} in {}".format(self.Line,self.InFile))
		self.Type = GetPoint(self.Type)

def AddParams(Item,Arr):
	Pos  = 0
	while Item.Extra[Pos].Value != ":=":
		Pos += 1
	for i in range(0,Pos,2):
		PreAdd = ParamChain(None,Item.Extra[i])
		PreAdd.PreExtra = Item.Extra[Pos+1]
		Arr.append(PreAdd)

class BoxFakeMetod:
	def __init__(self,Obj):
		self.Value = "~mimic"
		self.Type = None
		self.Pos = -1
		self.ToCall = None
		self.Id = GetNumb()
		self.PreType = None
		self.PrevType = None
		self.InClass = None
		self.IsPoint = False
		self.IsGlobal = False
		if type(Obj) is type(""):
			self.ToCall = Obj
			self.Name = Obj
		elif Obj.Value == "fakeparam":
			self.Name = Obj.Extra[0].Extra
			self.PreType = Obj.Extra[2]
			self.ToCall = Obj.Extra[4].Extra #TODO
		else:
			print("Not now") # TODO
			
	def PrintInBlock(self,F,ToUse,UseId = -1):
		EndId = -1
		PreId = -1
		if self.PreType != None or self.Type.Type == "fixed":
			if UseId == -1:
				EndId = self.Id
				PreId = GetNumb()
			else:
				EndId = UseId
				PreId = GetNumb()
		else:
			if UseId == -1:
				PreId = self.Id
			else:
				PreId = UseId
	
		if ToUse.Type.IsPoint:
			ToUse.PrintPre(F)
		else:
			ToUse.PrintPointPre(F)
		F.write("%Tmp{} = getelementptr ".format(PreId))
		if ToUse.Type.IsPoint:
			ToUse.Type.Base.PrintUse(F)
		else:
			ToUse.Type.PrintUse(F)
		F.write(",")
		if ToUse.Type.IsPoint:
			ToUse.PrintUse(F)
		else:
			ToUse.PrintPointUse(F)
		#ToUse.PrintPointUse(F)
		F.write(", i32 0, i32 {}\n".format(self.Pos))
		if self.PreType != None or self.Type.Type == "fixed":
			F.write("%Tmp{} = bitcast ".format(EndId))
			if self.Type.Type == "fixed":
				if self.PrevType == None:
					self.Type.PrintInAlloc(F)
				else:
					self.PrevType.PrintInAlloc(F)
				F.write("*")
			else:
				GetPoint(self.PrevType).PrintUse(F)
			F.write(" %Tmp{}".format(PreId))
			F.write(" to ")
			GetPoint(self.Type).PrintUse(F)
			F.write("\n")
	def GetName(self,UseId = -1,BoxId = -1):
		if UseId == -1:
			UseId = self.PrevId
		return "%Tmp{}".format(UseId)
	def GetPName(self,UseId = -1,BoxId = -1):
		if UseId == -1:
			#UseId = self.PrevId
			BoxId = self.Id
		return "%Tmp{}".format(BoxId)
	def PrintPointPre(self,F,UseId = -1,BoId = -1):
		return None
	def PrintPointUse(self,F,UseId = -1,BoId = -1):
		if UseId == -1:
			#UseId = self.PrevId
			BoId = self.Id
		GetPoint(self.Type).PrintUse(F)
		F.write(" %Tmp{}".format(BoId))
		return None
	def PrintUse(self,F,UseId = -1,BoId = -1):
		if UseId == -1:
			UseId = self.PrevId
			BoId = self.Id
		if self.Type.Type == "fixed":
			self.PrintPointUse(F,UseId,BoId)
			return None
		self.Type.PrintUse(F)
		F.write(" %Tmp{}".format(UseId))
	def PrintPre(self,F,UseId = -1,BoId = -1):
		if UseId == -1:
			self.PrevId = GetNumb()
			UseId = self.PrevId
			BoId = self.Id
		
		if self.Type.Type == "fixed":
			self.PrintPointPre(F,UseId,BoId)
			return None
		#self.PrevId = GetNumb()
		F.write("%Tmp{} = load ".format(UseId))
		self.Type.PrintUse(F)
		F.write(",")
		GetPoint(self.Type).PrintUse(F)
		F.write(" %Tmp{}\n".format(BoId))
	def Check(self,Paren = None):
		self.Pos = self.InClass.GetPos(self.ToCall)
		ExtrChk = self.Pos
		if self.InClass.VirtualFuncs != None:
			ExtrChk -= 1
		if self.Pos < 0:
			raise ValueError("There is no object {}".format(self.ToCall))
		self.InClass.Items[ExtrChk].Check()
		if self.PreType == None:
			self.Type = self.InClass.Items[ExtrChk].Type 
		else:
			self.PrevType = self.InClass.Items[ExtrChk].Type
			self.Type = ParseType(self.PreType)
		
	
class BoxTemplate:
	def __init__(self,Obj):
		self.Value = "~templ"

		self.SubClasses = []
		self.ParsTable = []

		self.ParsNames = []
		self.Name = Obj.Extra[0].Extra
		self.Type = "template"
		
		for it in Obj.Extra[3].Extra[1].Extra:
			if it.Value == "id":
				self.ParsNames.append(it.Extra)
		Obj.Extra.pop(3)
		self.Extra = Obj
	def PrintFunc(self,F):
		for it in self.SubClasses:
			it.PrintFunc(F)
	def PrintConst(self,F):
		for it in self.SubClasses:
			it.PrintConst(F)
	def GetClass(self,Pars):
		if len(self.ParsNames) != len(Pars):
			return None
		for i in range(len(self.ParsTable)):
			WrongValue = False
			for j in range(len(Pars)):
				if self.ParsTable[i][j].Id != Pars[j].Id:
					WrongValue = True
					break
			if not WrongValue:
				return self.SubClasses[i]
		PushT()
		for i in range(len(Pars)):
			AddDef(self.ParsNames[i],Pars[i])
		NewCls = BoxClass(self.Extra)
		self.SubClasses.append(NewCls)
		self.ParsTable.append(Pars)
		NewCls.Check()
		PopT()
		return NewCls
			
	def Check(self):
		return None


class BoxVTable:
	def __init__(self):
		self.Id = GetNumb()
		self.Funcs = []
	def GetVFuncPos(self,Res,Pars):
		SomeFunc =  SearchFunc(Res,Pars,self.Funcs,True)
		if SomeFunc == None:
			return None
		for i in range(len(self.Funcs)):
			if self.Funcs[i] == SomeFunc:
				return i
		return None
	def TryReplace(self,NewFunc):
		j = self.GetVFuncPos(NewFunc.Name,NewFunc.Params)
		if j == None:
			self.Funcs.append(NewFunc)
		else:
			self.Funcs[j] = NewFunc
	def CloneIt(self):
		Cl = BoxVTable()
		for it in self.Funcs:
			Cl.Funcs.append(it)
		return Cl
	def PrintConst(self,F):
		F.write("%Class{} = type ".format(self.Id))
		F.write("{")
		for i in range(len(self.Funcs)):
			if i > 0:
				F.write(" , ")
			self.Funcs[i].PrintFType(F)
			F.write("*")
		F.write("}\n")
		F.write("@ClassStandart{0} = global %Class{0}".format(self.Id))
		F.write(" {")
		for i in range(len(self.Funcs)):
			if i > 0:
				F.write(" , ")
			self.Funcs[i].PrintFType(F)
			F.write("* ")
			self.Funcs[i].PrintName(F)
		F.write("}\n")
	def PrintBType(self,F):
		F.write("%Class{}".format(self.Id))
	def PrintType(self,F):
		F.write("%Class{}*".format(self.Id))
	def PrintUse(self,F):
		F.write("%Class{0}* @ClassStandart{0}".format(self.Id))
		

class BoxClass:
	def __init__(self,Obj):
		self.Name = Obj.Extra[0].Extra
		self.Type = "class"
		self.Value = "~class"
		self.Point = None
		self.Array = None
		self.Fixs = []
		self.IsPoint = False
		self.Items = []
		self.Funcs = []
		self.Fakes = []
		self.Id = GetNumb()
		self.ToExtend = None
		self.IsChecked = False
		self.VirtualFuncs = None
		
		if Obj.Extra[3].Value == "extend":
			self.ToExtend = Obj.Extra[4]	
		Stuf = Obj.Extra[len(Obj.Extra)-1].Extra
		for i in range(len(Stuf)):
			if Stuf[i].Value == "newparam":
				if Stuf[i].Extra[2].Value != "!()":
					self.Items.append(ParamChain(Stuf[i]))
				else:
					PreAdd = BoxFunc(Stuf[i].Extra[2],self)
					if Stuf[i].Extra[0].Value == "this":
						Stuf[i].Extra[0].Extra = "this"
					PreAdd.Name = Stuf[i].Extra[0].Extra
					self.Funcs.append(PreAdd)
			elif Stuf[i].Value == "newparams":
				AddParams(Stuf[i],self.Items)
			elif Stuf[i].Value == "fakeparam":
				self.Fakes.append(BoxFakeMetod(Stuf[i])) #print("Hello")
		for It in self.Items:
			self.Fakes.append(BoxFakeMetod(It.Name))
		for It in self.Fakes:
			It.InClass = self
					
	def Check(self):
		if self.IsChecked:
			return None
		self.IsChecked = True
		if self.ToExtend != None:
			self.ToExtend = ParseType(self.ToExtend)
			if self.ToExtend == None or self.ToExtend.Value != "~class":
				raise ValueError("Nothing to extend")
			self.ToExtend.Check()
			self.Items = self.ToExtend.Items  + self.Items
			self.Fakes = self.ToExtend.Fakes  + self.Fakes
			if self.ToExtend.VirtualFuncs != None:
				self.VirtualFuncs = self.ToExtend.VirtualFuncs.CloneIt()
		if self.VirtualFuncs == None:
			ContainVirt = False
			for It in self.Funcs:
				if It.IsVirtual:
					ContainVirt = True
			if ContainVirt:
				self.VirtualFuncs = BoxVTable()
		for It in self.Items:
			It.Check()
		for It in self.Fakes:
			It.Check(self)
		for It in self.Fakes:
			ContextStuff.append(It)
		for It in self.Funcs:
			for k in It.Params:
				k.Check()
			if self.VirtualFuncs != None and It.IsVirtual:
				self.VirtualFuncs.TryReplace(It)
		for It in self.Funcs:
			It.Check()
	def PrintInAlloc(self,F):
		F.write("%Class{}".format(self.Id))
	def PrintConst(self,F):
		if self.VirtualFuncs != None:
			self.VirtualFuncs.PrintConst(F)
		for It in self.Items:
			It.PrintConst(F)
		for It in self.Funcs:
			It.PrintConst(F)
		F.write("%Class{} = type {}".format(self.Id,"{"))
		if self.VirtualFuncs != None:
			self.VirtualFuncs.PrintType(F)
			if len(self.Items) != 0:
				F.write(",")
		for i in range(len(self.Items)):
			if i >= 1:
				F.write(" , ")
			self.Items[i].Type.PrintInAlloc(F)
		F.write("}\n")
	def AppendFakes(self):
		for It in self.Fakes:
			ContextStuff.append(It)
	def UseFakes(self,F,Param):
		for It in self.Fakes:
			It.PrintInBlock(F,Param)
	def PrintFunc(self,F):
		for It in self.Items:
			It.PrintFunc(F)
		for It in self.Funcs:
			It.PrintFunc(F)
	def PrintUse(self,F):
		F.write("%Class{}".format(self.Id))
		return None
	def GetName(self):
		return "%Class{}".format(self.Id)
	def InFamily(self,Item):
		if Item.Id == self.Id:
			return True
		if self.ToExtend != None:
			return self.ToExtend.InFamily(Item)
		return False
	def GetFunc(self,Res,Pars):
		SomeFunc =  SearchFunc(Res,Pars,self.Funcs,True)
		if SomeFunc != None:
			return SomeFunc
		if self.ToExtend != None:
			return self.ToExtend.GetFunc(Res,Pars)
		return None
		
	def GetPos(self,Res):
		ItPos = -1
		for i in range(len(self.Items)):
			if self.Items[i].Name == Res:
				ItPos = i
		if ItPos == -1:
			return -1
		if self.VirtualFuncs != None:
			ItPos += 1
		return ItPos
	def GetVFuncPos(self,Res,Pars):
		if self.VirtualFuncs != None:
			return self.VirtualFuncs.GetVFuncPos(Res,Pars)
		
	def GetFake(self,Res):
		for i in self.Fakes:
			if i.Name == Res:
				return i
		return None
	def PrintZero(self,F):
		self.PrintUse(F)
		F.write(" {")
		for i in range(len(self.Items)):
			if i > 0:
				F.write(",")
			self.Items[i].Type.PrintZero(F)
		F.write("}")
	def GetType(self,Res):
		for i in range(len(self.Items)):
			if self.Items[i].Name == Res:
				return self.Items[i].Type
		return -1
class ParConstStr:
    def __init__(self,Obj):
        self.Value = "~str"
        self.Extra = Obj.Extra
        self.Type = GetType("string")
        self.Size = 0
        self.Id = GetNumb()

        i = 0
	Doubls = 0
        while i < len(Obj.Extra):
            if Obj.Extra[i] == '\\':
                i += 1
            self.Size += 1
            i += 1
        self.Size += 1
	self.Size -= Doubls
    def PrintConst(self,F):
        F.write('@CStr{} = constant [{} x i8] c"'.format(self.Id,self.Size))
        #F.write('@CStr{0} = constant {{i32,[{1} x i8]}} {{i32 {1}, [{1} x i8] c"'.format(self.Id,self.Size))
        i = 0
        while i < len(self.Extra):
            if self.Extra[i] == '\\':
                i += 1
                if self.Extra[i] == 'n':
                    F.write('\\0A')
                    i+= 1
		elif self.Extra[i] == '"':
		    F.write('\\22')
		    i += 1
		elif self.Extra[i] == '\\':
		    F.write('\\\\')
		    i += 1
            else:
                F.write(self.Extra[i])
                i += 1
        F.write('\\00"\n')
        #F.write('\\00"}\n')
    def PrintFunc(self,F):
         return None
    def PrintPre(self,F):
        F.write("%LStr{0} = getelementptr [{1} x i8], [{1} x i8]* @CStr{0},i32 0,i32 0\n".format(self.Id,self.Size))
        #F.write("%LStr{0} = getelementptr {{i32,[{1} x i8]}}, {{i32,[{1} x i8]}}* @CStr{0},i32 0,i32 1, i32 0\n".format(self.Id,self.Size))
    def GetName(self):
        return "%LStr{}".format(self.Id)
    def PrintUse(self,F):
        F.write("i8* %LStr{}".format(self.Id))
    def Check(self):
        return None
    def PrintAlloc(self,F):
	return None


class ParConstNum:
    def __init__(self,Obj):
        self.Value = "~num"
        self.Extra = Obj.Extra
        self.PrevId = -1
        
        if Obj.Value in ["true","false"]:
            self.Type = GetType("bool")
            self.Extra = (1 if Obj.Value == "true" else 0)
        if Obj.Value == "numi":
            self.Type = GetType("int")
        if Obj.Value == "numf":
            self.Type = GetType("float") #GetType("float")i
	if Obj.Value == "null":
	    self.Type = GetPoint(GetType("void"))
	    self.Extra = 0
    def Check(self):
        return None
    def PrintAlloc(self,F):
	return None
    def PrintConst(self,F):
        if self.Extra != None and hasattr(self.Extra,"PrintConst"):
            self.Extra.PrintConst(F)
        return None
    def PrintFunc(self,F):
        return None
    def PrintAsConst(self,F):
	if self.Type.Id == GetType("float").Id:
        	F.write(" float {}\n".format(self.Extra))
	else:
		self.Type.PrintUse(F)
		F.write(" {}".format(self.Extra))
	return None
    def GetName(self):
	if self.Type.Id == GetVoidP():
		return "null"
	else:
        	return "%Tmp{}".format(self.PrevId)
    def PrintPre(self,F):
        self.PrevId = GetNumb()
        if self.Type.Id == GetType("int").Id or self.Type.Id == GetType("bool").Id:
            F.write("%Tmp{} = add ".format(self.PrevId))
            self.Type.PrintUse(F)
            F.write(" {} , 0\n".format(self.Extra))
        elif self.Type.Id == GetVoidP():
	    return None
	else:
            F.write("%Tmp{} = fptrunc double {} to float\n".format(self.PrevId,self.Extra))

    def PrintUse(self,F):
	if self.Type.Id == GetVoidP():
		self.Type.PrintUse(F)
		F.write(" null")
	else:
        	self.Type.PrintUse(F)
        	F.write(" %Tmp{}".format(self.PrevId))
        return None

class BoxNewType:
    def __init__(self,Obj):
        self.Value = "~newtype"
	self.Type = None
	self.PreType = Obj.Extra[3]
	self.Name = Obj.Extra[0].Extra
    def Check(self):
	self.Type = ParseType(self.PreType)
	if self.Type == None:
		raise ValueError("not found type")
	AddDef(self.Name,self.Type)
        return None
    def PrintAlloc(self,F):
	return None
    def PrintConst(self,F):
	return None
    def PrintFunc(self,F):
        return None
    def PrintAsConst(self,F):
	return None
    def GetName(self):
	return None
    def GetName(self):
	return None
    def GetPName(self):
	return None
    def PrintPre(self,F):
	return None
    def PrintPointPre(self,F):
	return None
class ParRangeNum:
    def __init__(self,Obj):
        self.Value = "~d..d"
	self.L = GetUse(Obj.Extra[0])
	self.R = GetUse(Obj.Extra[2])
	self.IsConst = (self.L.Value == "~num") and (self.R.Value == "~num")
        self.PrevId = -1
        self.Type = GetFixedArr(GetType("u32"),32)
	self.Id = GetNumb()
	
	self.Extra = [0,0,0,0,0,0,0,0]
	if self.IsConst and self.L.Type.Id == GetType("int").Id and self.R.Type.Id == GetType("int").Id:
		for i in range(self.L.Extra,self.R.Extra+1):
			Md = i % 32
			Dv = (i - Md) / 32
			self.Extra[Dv] += 2**Md
    def Check(self):
        return None
    def PrintAlloc(self,F):
	return None
    def PrintConst(self,F):
	F.write("@Tmp{0} = constant [8 x i32] ".format(self.Id))
	F.write("[")
	for i in range(8):
		if i != 0:
			F.write(" , ")
		F.write("i32 {}".format(self.Extra[i]))		
	F.write("] \n")
    def PrintFunc(self,F):
        return None
    def PrintAsConst(self,F):
	return None
    def GetName(self):
	if self.IsConst:
		return "@Tmp{}".format(self.Id)
	else:
		raise ValueError("not implemented")
    def GetPName(self):
	if self.IsConst:
		return "@Tmp{}".format(self.Id)
	else:
		raise Value("not implemented")
    def PrintPre(self,F):
	return None
	raise ValueError("not implemented")
    def PrintPointPre(self,F):
	return None
	raise ValueError("not implemented")
	

class BoxReturn:
	def __init__(self,Obj):
		self.Value = "~ret"
		self.Ret = GetUse(Obj.Extra[1])
		self.RetRef = False
		self.ToCall = None
		self.Line = Obj.Line
	def PrintConst(self,F):
		self.Ret.PrintConst(F)
		return None
	def PrintFunc(self,F):
		return None
	def Check(self):
		if self.Ret != None:
			self.Ret.Check()
		L = InFunc[len(InFunc) -1]
		self.RetRef = L.RetRef
		self.RetType = L.Type
		if self.Ret.Type.Id != L.Type.Id and (GoodPoints(self.Ret.Type, L.Type) or GoodPoints(L.Type,self.RetType)):
			self.Ret = BoxExc(self.Ret,L.Type)
		if self.Ret.Type.Type == "point" and L.Type.Type == "point":
			if self.Ret.Type.Base.Type == "class" and L.Type.Base.Type == "class":
				if self.RetType.Base.InFamily(L.Type.Base):
					self.Ret = BoxExc(self.Ret,L.Type)
					
		if L.ReturnInInput != None:
			self.RetParam = GetUse(Token("id","ret"))
			self.RetParam.Check()
			self.ToCall = GetFunc("=",[self.RetParam,self.Ret])
			if self.ToCall == None:
				if L.Type.Type == "class":
					self.ToCall = L.Type.GetFunc("=",[self.RetParam,self.Ret])
				if self.ToCall == None:
					raise ValueError("can not return value at line {}".format(self.Line))
	def PrintPre(self,F):
		if self.ToCall != None:
			self.RetParam.PrintPointPre(F)
			if self.ToCall.Params[1].IsRef:
				self.Ret.PrintPointPre(F)
			else:
				self.Ret.PrintPre(F)
		return None
	def PrintAlloc(self,F):
		return None
	def PrintUse(self,F):
		if self.ToCall != None:
			self.ToCall.PrintUse(F,GetNumb(),[self.RetParam,self.Ret])
			return None
		if self.RetRef:
			self.Ret.PrintPointPre(F)
			F.write("ret ")
			self.Ret.PrintPointUse(F)
			F.write("\n")
			return None
		self.Ret.PrintPre(F)
		F.write("ret ")
		self.Ret.PrintUse(F)
		F.write("\n")
		return None
	def PrintInBlock(self,F):
		self.PrintUse(F)

class BoxMetodCall:
    def __init__(self,Obj):
        self.Value = "~id"
        self.Type = GetType("void")
        self.PrevId = None
        self.Object = None
	self.Id = GetNumb()
	self.IsPoint = False
	
	if Obj.Value == "d.d":
		self.Param = GetUse(Obj.Extra[0])
		self.ToCall = Obj.Extra[2].Extra	
        
    def PrintConst(self,F):
        return None
    def PrintFunc(self,F):
        return None
    def PrintPre(self,F):
	self.PrevId = GetNumb()
	self.BoxId = GetNumb()
	self.ToUse.PrintInBlock(F,self.Param,self.BoxId)
	self.ToUse.PrintPre(F,self.PrevId,self.BoxId)
    def GetPName(self):
	return self.ToUse.GetPName(self.PrevId,self.BoxId)
        #return "%Tmp{}".format(self.Id)
    def GetName(self):
	return self.ToUse.GetName(self.PrevId,self.BoxId)
    def PrintUse(self,F):
	self.ToUse.PrintUse(F,self.PrevId,self.BoxId)
    def PrintInBlock(self,F):
        return None
    def PrintAlloc(self,F):
	self.Param.PrintAlloc(F)
	return None
    def PrintPointPre(self,F):
	self.PrevId = GetNumb()
	self.BoxId = GetNumb()
	self.ToUse.PrintInBlock(F,self.Param,self.BoxId)
	self.ToUse.PrintPointPre(F,self.PrevId,self.BoxId)
    def PrintPointUse(self,F):
	self.ToUse.PrintPointUse(F,self.PrevId,self.BoxId)
    def Check(self):
	self.Param.Check()
        self.ClassType = self.Param.Type
	if self.ClassType.Type != "class":
		if self.ClassType.Base.Type != "class":
			raise ValueError("Not a class")
		else: 
			self.UseClass = self.ClassType.Base
	else:
		self.UseClass = self.ClassType
	self.UseClass.Check()
	self.ToUse = self.UseClass.GetFake(self.ToCall)
	if self.ToUse == None:
		print(len(self.UseClass.Items))
		print(self.UseClass.Items[0].Name)
		raise ValueError("Object does not have field {}".format(self.ToCall))
	self.ToUse.Check()
	self.Type = self.ToUse.Type
class BoxParamCall:
    def __init__(self,Obj):
        self.Value = "~id"
        self.Type = GetType("void")
        self.PrevId = None
        self.Object = None
        self.ToUse = Obj.Extra
	self.Line = Obj.Line
	if Obj.Value == "this":
		self.ToUse = "this"
    def PrintConst(self,F):
        return None
    def PrintFunc(self,F):
        return None
    def PrintPre(self,F):
        #self.Object.Extra.PrintPre(F) 
        self.Object.PrintPre(F)
        if hasattr(self.Object,"PrevId"):
            self.PrevId = self.Object.PrevId
        return None
    def GetPName(self):
	return self.Object.GetPName()
    def GetName(self):
	return self.Object.GetName()
    def PrintUse(self,F):
        self.Object.PrintUse(F)
        return None
    def PrintAlloc(self,F):
	return None
    def PrintInBlock(self,F):
        return None
    def Check(self):
        self.Object = GetParam(self.ToUse)
        if self.Object == None:
            raise ValueError("Object not found {} at line {}".format(self.ToUse,self.Line))
	self.Object.Check()
        self.Type = self.Object.Type
        self.Id = self.Object.Id
        #self.Type = self.CallFunc.RetType
        return None
    def PrintPointPre(self,F):
    	self.Object.PrintPointPre(F)
        return None
    def PrintPointUse(self,F):
	self.Object.PrintPointUse(F) #TODO: check
        #GetPoint(self.Type).PrintUse(F)
	#if self.Object.IsGlobal:
        #	F.write(" @Tmp{}".format(self.Id))
	#else:
        #	F.write(" %Tmp{}".format(self.Id))
        return None

class BoxRef:
	def __init__(self,Obj):
		self.Value = "~d&"
		self.Extra = GetUse(Obj.Extra[0])
	def PrintPre(self,F):
		self.Extra.PrintPointPre(F)
	def PrintUse(self,F):
		self.Extra.PrintPointUse(F)
	def Check(self):
		self.Extra.Check()
		self.Type = GetPoint(self.Extra.Type)
	def PrintConst(self,F):
		self.Extra.PrintConst(F)
	def PrintFunc(self,F):
		self.Extra.PrintFunc(F)
	def PrintAlloc(self,F):
		return None
		self.Extra.PrintAlloc(F)

class BoxRefCandy:
	def __init__(self,Obj):
		self.Value = "ref x"
		self.Extra = Obj
		self.Type = Obj.Type.Base
	def PrintPointPre(self,F):
		self.Extra.PrintPre(F)
	def PrintPointUse(self,F):
		self.Extra.PrintUse(F)
	def Check(self):
		self.Extra.Check()
		self.Type = GetPoint(self.Extra.Type)
	def PrintConst(self,F):
		self.Extra.PrintConst(F)
	def PrintFunc(self,F):
		self.Extra.PrintFunc(F)
	def PrintAlloc(self,F):
		self.Extra.PrintAlloc(F)
	
	

class BoxPoint:
    def __init__(self,Obj):
	self.FCall = None
        if Obj.Value == "d^":
            self.Value = "~d^"
            self.Extra = GetUse(Obj.Extra[0])
        if Obj.Value == "d[]":
            self.Value = "~d[]"
            self.Extra = GetUse(Obj.Extra[0])
            self.Ind = GetUse(Obj.Extra[1].Extra[0])
    def PrintPointPre(self,F):
	if self.FCall != None:
		return self.FCall.PrintPointPre(F)
        if self.Value == "~d[]":
            self.PrevId = GetNumb()
            self.Extra.PrintPre(F)
            self.Ind.PrintPre(F)
            F.write("%Tmp{} = getelementptr ".format(self.PrevId))
            self.Type.PrintUse(F)
            F.write(" , ")
            self.Extra.PrintUse(F)
            F.write(" , ")
            self.Ind.PrintUse(F)
            F.write("\n")
        else:
            self.Extra.PrintPre(F)
    def PrintPointUse(self,F):

	if self.FCall != None:
		return self.FCall.PrintPointUse(F)

        if self.Value == "~d[]":
            self.Extra.Type.PrintUse(F)
            F.write(" %Tmp{}".format(self.PrevId))
        else:
            self.Extra.PrintUse(F)
    def PrintAlloc(self,F):
	self.Extra.PrintAlloc(F)
	if self.Value == "~d[]":
		self.Ind.PrintAlloc(F)
    def Check(self):
        self.Extra.Check()
        if self.Value == "~d[]":
            self.Ind.Check()
	
	if self.Value == "~d[]" and self.Extra.Type.Type in ["class","standart"]:
		PrePars = [self.Extra,self.Ind]
		self.FCall = BoxFuncsCall(None)
		self.FCall.ToCall = "[]"
		self.FCall.IsMetod = self.Extra.Type.Type == "class"
		self.FCall.Params = PrePars
		self.FCall.Check() 
		self.Type = self.FCall.Type
		return None
        elif not self.Extra.Type.IsPoint:
            	raise ValueError('Not a point')
        self.Type = self.Extra.Type.Base

    def GetName(self):
	if self.FCall != None:
		return self.FCall.GetName()
        return "%Tmp{}".format(self.PrevId)
    def GetPName(self):
	if self.FCall != None:
		return self.FCall.GetPName()
        if self.Value == "~d[]":
            return "%Tmp{}".format(self.PrevId)
        else:
            self.Extra.GetName()
    def PrintFunc(self,F):
        self.Extra.PrintFunc(F)
        if self.Value == "~d[]":
            self.Ind.PrintFunc(F)
    def PrintConst(self,F):
        self.Extra.PrintConst(F)
        if self.Value == "~d[]":
            self.Ind.PrintConst(F)
    def PrintPre(self,F):
	if self.FCall != None:
		return self.FCall.PrintPre(F)

        self.Extra.PrintPre(F)
        self.PrevId = GetNumb()
        if self.Value == "~d[]":
            self.Ind.PrintPre(F)
            F.write("%PreTmp{} = getelementptr ".format(self.PrevId))
            self.Type.PrintUse(F)
            F.write(" , ")
            self.Extra.PrintUse(F)
            F.write(" , ")
            self.Ind.PrintUse(F)
            F.write("\n")

        F.write("%Tmp{} = load ".format(self.PrevId))
        self.Type.PrintUse(F)
        F.write(" , ")
        if self.Value == "~d[]":
	    if self.Extra.Type.Type == "fixed":
            	GetPoint(self.Extra.Type).PrintUse(F)
	    else:
            	self.Extra.Type.PrintUse(F)
            F.write(" %PreTmp{}\n".format(self.PrevId))
        else:
            self.Extra.PrintUse(F)
            F.write("\n")
    def PrintUse(self,F):
	if self.FCall != None:
		return self.FCall.PrintUse(F)
        self.Type.PrintUse(F)
        F.write(" %Tmp{}".format(self.PrevId))

class BoxExc:
    def __init__(self,Obj,NewType = None):
        self.Value = "~[T]"
	if NewType == None:
		self.Extra = GetUse(Obj.Extra[0])
		self.Type = None
		self.PreType = Obj.Extra[2].Extra[0]
	else:
        	self.Extra = Obj
        	self.Type = NewType
		self.PreType = None
    def PrintPre(self,F):
        if self.Extra != None:
            self.Extra.PrintPre(F)
        self.PrevId = GetNumb()
        if self.Type.IsPoint:
            F.write("%Tmp{} = bitcast ".format(self.PrevId))
            self.Extra.PrintUse(F)
            F.write(" to {}\n".format(self.Type.GetName()))
        elif self.Type.Id == GetType("bool").Id:
		if self.Extra.Type.IsPoint:
            		F.write("%Tmp{} = icmp ne ".format(self.PrevId))
            		self.Extra.PrintUse(F)
            		F.write(" , null\n")
		else:
            		F.write("%Tmp{} = icmp ne ".format(self.PrevId))
            		self.Extra.PrintUse(F)
            		F.write(" , 0\n")
	elif self.Type.Id == GetType("double").Id:
		if self.Extra.Type.Id == GetType("float").Id:
			F.write("%Tmp{} = fpext ".format(self.PrevId))	
			self.Extra.PrintUse(F)
			F.write(" to double\n")
	else:
		ToUse = ShouldChange(self.Extra.Type,self.Type)
		if ToUse != "":
			if "f" in ToUse:
				if "s" in ToUse:
					F.write("%Tmp{} = sitofp ".format(self.PrevId))
					self.Extra.PrintUse(F)
					F.write(" to ")
					self.Type.PrintUse(F)
					F.write("\n")
				else: 
					F.write("%Tmp{} = uitofp ".format(self.PrevId))
					self.Extra.PrintUse(F)
					F.write(" to ")
					self.Type.PrintUse(F)
					F.write("\n")
			elif "i" in ToUse:
				if "s" in ToUse:
					F.write("%Tmp{} = fptosi ".format(self.PrevId))
					self.Extra.PrintUse(F)
					F.write(" to ")
					self.Type.PrintUse(F)
					F.write("\n")
				else: 
					F.write("%Tmp{} = fptoui ".format(self.PrevId))
					self.Extra.PrintUse(F)
					F.write(" to ")
					self.Type.PrintUse(F)
					F.write("\n")
			else:
				if "d" in ToUse:
					F.write("%Tmp{} = trunc ".format(self.PrevId))
					self.Extra.PrintUse(F)
					F.write(" to ")
					self.Type.PrintUse(F)
					F.write("\n")
				if "u" in ToUse:
					F.write("%Tmp{} = sext ".format(self.PrevId))
					self.Extra.PrintUse(F)
					F.write(" to ")
					self.Type.PrintUse(F)
					F.write("\n")
				if "z" == ToUse:		
					F.write("%Tmp{} = add ".format(self.PrevId))
					#self.Type.PrintUse(F)
					self.Extra.PrintUse(F)
					F.write(" , 0\n")
    def PrintUse(self,F):
        self.Type.PrintUse(F)
        F.write(" %Tmp{}".format(self.PrevId))
    def PrintPointPre(self,F):
	self.PrevId = GetNumb()
	if self.Type.IsPoint or self.Type.Type == "class":
		self.Extra.PrintPointPre(F)
		F.write("%Tmp{}  = bitcast ".format(self.PrevId))
		self.Extra.PrintPointUse(F)
		F.write(" to ")
		GetPoint(self.Type).PrintUse(F)
		F.write("\n")
	else:
		raise ValueError("Wrong use")
    def PrintPointUse(self,F):
	GetPoint(self.Type).PrintUse(F)
	F.write(" %Tmp{}".format(self.PrevId))
    def GetPName(self):
	return "%Tmp{}".format(self.PrevId)
    def GetName(self):
	return "%Tmp{}".format(self.PrevId)
    def PrintConst(self,F):
        self.Extra.PrintConst(F)
    def PrintFunc(self,F):
        self.Extra.PrintFunc(F)
    def Check(self):
	self.Extra.Check()
	if self.PreType != None:
		self.Type = ParseType(self.PreType)
    def PrintAlloc(self,F):
	self.Extra.PrintAlloc(F)

class BoxFuncsCall:
    def __init__(self,Obj):
        self.Value = "~d()"
        self.Params = []
        self.Type = GetType("void")
        self.PrevId = None
        self.CallFunc = None
	self.IsMetod = False
	self.PointCall = None
        self.HaveConstr = None
	self.RetRef = False
	self.RetId = -1
	self.ReserveItem = False
	self.VirtualCall = None

	if Obj != None:
		self.Line = Obj.Line
		self.InFile = Obj.InFile
	else:
		self.Line = -1
		self.InFile = "error"

	if Obj == None:
		return None
        
        if Obj.Value == "d()" and Obj.Extra[0].Value == "id": 
            self.ToCall = Obj.Extra[0].Extra
            Pars = Obj.Extra[1].Extra
            if len(Pars) != 0:
                i = 0
                while i < len(Pars):
                    if Pars[i].Value != ',':
                        self.Params.append(GetUse(Pars[i]))                    
                    i += 1
        elif Obj.Value == "d.d()":
	    self.ToCall = Obj.Extra[2].Extra
	    self.Params.append(GetUse(Obj.Extra[0]))
            Pars = Obj.Extra[3].Extra
	    self.IsMetod = True
            if len(Pars) != 0:
                i = 0
                while i < len(Pars):
                    if Pars[i].Value != ',':
                        self.Params.append(GetUse(Pars[i]))                    
                    i += 1
	else:
            self.ToCall = Obj.Extra[1].Value
            if Obj.Extra[0].Value == "not" or Obj.Extra[0].Value == "-":
                self.Params.append(GetUse(Obj.Extra[1]))
                self.ToCall = Obj.Extra[0].Value
            else:
                self.Params.append(GetUse(Obj.Extra[0]))
                self.Params.append(GetUse(Obj.Extra[2]))

    def PrintConst(self,F):
        for P in self.Params:
            P.PrintConst(F)
    def PrintFunc(self,F):
        for P in self.Params:
            P.PrintFunc(F)
    def PrintAlloc(self,F):
	for It in self.Params:
		It.PrintAlloc(F)
	if self.ReserveItem:
		F.write("%Tmp{} = alloca ".format(self.ConstrId))
		self.Type.PrintInAlloc(F)
		F.write("\n")	
    def PrintPre(self,F):
        self.PrevId = GetNumb()
	if self.RetRef:
		MiniPre = GetNumb()
		self.PrevId = GetNumb()
		self.CallFunc.PrintUse(F,MiniPre,self.Params)
		F.write("%Tmp{} = load ".format(self.PrevId))
		self.Type.PrintUse(F)
		F.write(" , ")
		GetPoint(self.Type).PrintUse(F)
		F.write(" %Tmp{}\n".format(MiniPre))
		return None
	if self.ReserveItem: #self.HaveConstr != None:
		NewParams = [self] + self.Params
		StartI = 1
	else:
		NewParams = self.Params
		StartI = 0
	
	FPars = []
	if self.CallFunc != None:
		FPars = self.CallFunc.Params
	if self.PointCall != None:
		FPars = self.PointCall.Params

	for i in range(StartI,len(NewParams)):
		if i < len(FPars) and FPars[i].IsRef:
        		NewParams[i].PrintPointPre(F)
		else:
        		NewParams[i].PrintPre(F)
	if self.CallFunc == None or self.CallFunc.AsmLine == None: 
        	#self.PrevId = GetNumb()
		if self.PointCall != None:
			self.PointCall.PrintPre(F)
		if self.VirtualCall != None:
			if self.CallFunc.ReturnInInput == None or True: #???
				MyPar0 = self.Params[0]
			else:
				MyPar0 = self.Params[1]

			MyPar0.PrintPointPre(F)
			F.write("%FuncPos{} = getelementptr ".format(self.PrevId))
			MyPar0.Type.PrintInAlloc(F)
			F.write(" , ")
			MyPar0.PrintPointUse(F)
			F.write(", i32 0, i32 0\n")
			F.write("%VTabl{} = load ".format(self.PrevId))
			MyPar0.Type.VirtualFuncs.PrintType(F)
			F.write(" , ")
			MyPar0.Type.VirtualFuncs.PrintType(F)
			F.write("* %FuncPos{}\n".format(self.PrevId))
			F.write("%MyFuncP{} = getelementptr ".format(self.PrevId))
			MyPar0.Type.VirtualFuncs.PrintBType(F)
			F.write(" , ")
			MyPar0.Type.VirtualFuncs.PrintBType(F)
			F.write("* %VTabl{},i32 0,i32 {}\n".format(self.PrevId,self.VirtualCall))
			F.write("%FuncCall{} = load ".format(self.PrevId))
			self.CallFunc.PrintType(F)		
			F.write("* , ")	
			self.CallFunc.PrintType(F)			
			F.write("** %MyFuncP{}\n".format(self.PrevId))
		if self.Type.Id != GetType("void").Id and not self.ReserveItem:
			F.write("%Tmp{} = ".format(self.PrevId))
		F.write("call ")
		if self.PointCall == None:
			if self.VirtualCall != None:
				self.CallFunc.PrintFType(F)
				F.write("%FuncCall{}".format(self.PrevId))
			else:
				self.CallFunc.PrintFType(F)
				self.CallFunc.PrintName(F)
			if self.CallFunc.ReturnInInput != None:
				Pars = [self.CallFunc.ReturnInInput] +self.CallFunc.Params
			else:
				Pars = self.CallFunc.Params
		else:
			self.Type.PrintUse(F)
			F.write(" {}".format(self.PointCall.GetName()))
			#F.write("%Tmp{}".format(MiniId))
			Pars = self.PointCall.Type.Params
		F.write("(")
		for i in range(len(NewParams)):
			if i > 0:
				F.write(",")
			if i < len(Pars) and self.PointCall == None and Pars[i].IsRef:
        			NewParams[i].PrintPointUse(F)
			else:
        			NewParams[i].PrintUse(F)
		F.write(")\n")
		
	else:
		if len(NewParams) == 1:
			if self.CallFunc.Params[0].IsRef:
				F.write(self.CallFunc.AsmLine.format("%Tmp{}".format(self.PrevId),NewParams[0].GetPName()))
			else:
				F.write(self.CallFunc.AsmLine.format("%Tmp{}".format(self.PrevId),NewParams[0].GetName()))
		if len(NewParams) == 2:
			if self.CallFunc.Params[0].IsRef:
				F.write(self.CallFunc.AsmLine.format("%Tmp{}".format(self.PrevId),NewParams[0].GetPName(),NewParams[1].GetName()))
			else:
				F.write(self.CallFunc.AsmLine.format("%Tmp{}".format(self.PrevId),NewParams[0].GetName(),NewParams[1].GetName()))
        return None
    def GetName(self):
        return "%Tmp{}".format(self.PrevId)
    def PrintUse(self,F):
        self.Type.PrintUse(F)
        F.write(" %Tmp{}".format(self.PrevId))
        return None
    def PrintInBlock(self,F):
        self.PrintPre(F)
    def PrintPointPre(self,F):
	if self.RetRef:
		self.PrevId = GetNumb()
		self.CallFunc.PrintUse(F,self.PrevId,self.Params)
		return None
	if self.ReserveItem:
		self.PrintPre(F)
	return None
    def PrintPointUse(self,F):
    	if self.RetRef:
		GetPoint(self.Type).PrintUse(F)
		F.write(" %Tmp{}".format(self.PrevId))
		return None
	GetPoint(self.Type).PrintUse(F)
	F.write(" %Tmp{}".format(self.ConstrId))
    def GetPName(self):
	if self.RetRef:
		return "%Tmp{}".format(self.PrevId)
	return "%Tmp{}".format(self.ConstrId)
    def Check(self):
        for It in self.Params:
            It.Check()
	if self.ToCall == "block":
		return None
	if self.ToCall in CmprOpers and self.Params[0].Type.Type == "standart" and self.Params[1].Type.Type == "standart":
		if self.Params[0].Type.Id != self.Params[1].Type.Id:
			TSome = BestType(self.Params[0].Type,self.Params[1].Type)
			for i in [0,1]:
				if self.Params[i].Type.Id != TSome.Id:
					self.Params[i] = BoxExc(self.Params[i],TSome)
	if self.ToCall in Opers and len(self.Params) == 2 and self.Params[0].Type.Type == "standart" and self.Params[1].Type.Type == "standart":
		if self.Params[0].Type.Id != self.Params[1].Type.Id:
			TSome = BestType(self.Params[0].Type,self.Params[1].Type)
			for i in [0,1]:
				if self.Params[i].Type.Id != TSome.Id:
					self.Params[i] = BoxExc(self.Params[i],TSome)
	#if self.ToCall in Prisv
	if self.ToCall in PrisvOpers and self.Params[0].Type.Type == "standart" and self.Params[1].Type.Type == "standart":
		if self.Params[0].Type.Id != self.Params[1].Type.Id:
			self.Params[1] = BoxExc(self.Params[1],self.Params[0].Type)
		
	if self.IsMetod or (self.ToCall in AllOpers and self.Params[0].Type.Type == "class"):
		if self.Params[0].Type.Type == "class" or (self.Params[0].Type.Type == "point" and self.Params[0].Type.Base.Type == "class"):
			CType = self.Params[0].Type if self.Params[0].Type.Type == "class" else self.Params[0].Type.Base
			CType.Check()
        		self.CallFunc = CType.GetFunc(self.ToCall,self.Params)
		if self.CallFunc == None:
			self.CallFunc = GetFunc(self.ToCall , self.Params, True)
		if self.CallFunc != None and self.CallFunc.IsVirtual:
			self.VirtualCall = self.CallFunc.InClass.GetVFuncPos(self.ToCall,self.Params)
	else:
        	self.CallFunc = GetFunc(self.ToCall,self.Params)
		if self.CallFunc == None and self.ToCall not in AllOpers:
			self.PointCall = GetParam(self.ToCall,True)
			if self.PointCall == None and InFunc[len(InFunc) - 1].InClass != None:
				AndThis = GetUse(Token("this","this"))
				AndThis.Check()
				self.CallFunc = InFunc[len(InFunc) -1].InClass.GetFunc(self.ToCall, [AndThis] + self.Params)
				if self.CallFunc != None:
					self.Params = [AndThis] + self.Params
					self.IsMetod = True
					if  self.CallFunc.IsVirtual:
						self.VirtualCall = self.CallFunc.InClass.GetVFuncPos(self.ToCall,self.Params)
				
	if self.CallFunc != None:
		self.CallFunc.Check()
		self.RetRef = self.CallFunc.RetRef
		self.Type = self.CallFunc.Type
		if self.CallFunc.ReturnInInput != None:
			self.ReserveItem = True
			self.ConstrId = GetNumb()		
        if self.CallFunc == None and self.PointCall == None:
		self.HaveConstr = ParseType(Token("id",self.ToCall))
		if self.HaveConstr != None:
			self.ReserveItem = True
			self.Type = self.HaveConstr
			self.CallFunc = self.HaveConstr.GetFunc("this",[self] +  self.Params)
			
			if self.CallFunc == None:
				PreError = "Func not found {}".format(self.ToCall)
				for it in self.Params:
					PreError += "\nParam {}".format(it.Type.GetName())
            			raise ValueError(PreError + " at {} in {}".format(self.Line,self.InFile))

			self.ConstrId = GetNumb()
		else:
			PreError = "Func not found {} at line {} file {}".format(self.ToCall,self.Line,self.InFile)
			for it in self.Params:
				PreError += "\nParam {}".format(it.Type.GetName())
				PreError += " {}".format(it.Type.Id)
			CloseFunc = SearchFuncClose(self.ToCall,ContextStuff)
			if CloseFunc != None:
				PreError += "\nclosest params"
				for it in CloseFunc.Extra.Params:
					if it.Type == None:
						PreError += "\nParam unknown"
					else:
						PreError += "\nParam {} {}".format(it.Type.GetName(),it.Type.Id)
					
            		raise ValueError(PreError)
        #self.Ret = self.CallFunc
	if self.HaveConstr == None:
		if self.PointCall != None:
			self.Type = self.PointCall.Type.Base 
		else:
	        	self.Type = self.CallFunc.Type
	
	i = 0
	if self.HaveConstr == None:
		NewParams = self.Params
	else: 
		NewParams = [self] + self.Params

	if self.PointCall != None:
		CheckParams = self.PointCall.Type.Params
	else:
		CheckParams = self.CallFunc.Params

	CheckParamsType = []
	if len(CheckParams) > 0:
		if type(CheckParams[0].Type) is not type(""):
			for i in range(len(CheckParams)):
				CheckParamsType.append(CheckParams[i].Type)
		else:
			for i in range(len(CheckParams)):
				CheckParamsType.append(CheckParams[i])
	if len(NewParams) != len(CheckParams) and self.CallFunc != None and self.CallFunc.Vargs != True:
		raise ValueError("Wrong input for function " + self.ToCall)

        for i in range(len(NewParams)):
            if CheckParamsType[i] == GetType("..."):
		while i < len(NewParams):
			if NewParams[i].Type.Id == GetType("float").Id:
				NewParams[i] = BoxExc(NewParams[i],GetType("double"))
			i += 1
            	break
	    if type(CheckParamsType[i]) is type(""):
		print(i)
		print(self.PointCall) #TODO: delete
	    if CheckParamsType[i].Id == NewParams[i].Type.Id:
		continue
            if CheckParamsType[i].Id != NewParams[i].Type.Id and GoodPoints(NewParams[i].Type,CheckParamsType[i]):
                NewParams[i] = BoxExc(NewParams[i],CheckParamsType[i])
		continue
            if CheckParamsType[i].Id != NewParams[i].Type.Id and GoodPoints(CheckParamsType[i],NewParams[i].Type):
                NewParams[i] = BoxExc(NewParams[i],CheckParamsType[i])
		continue
	    if NewParams[i].Type.Type == "class":
		NewParams[i] = BoxExc(NewParams[i],CheckParamsType[i])
		continue
	    if NewParams[i].Type.Type == "point" and CheckParamsType[i].Type == "point":
	    	if NewParams[i].Type.Base.Type == "class" and CheckParamsType[i].Base.Type == "class":
	    		if NewParams[i].Type.Base.InFamily(CheckParamsType[i].Base):
				NewParams[i] = BoxExc(NewParams[i],CheckParamsType[i])
		continue
	    if "u" in ShouldChange(NewParams[i].Type,CheckParamsType[i]):
		NewParams[i] = BoxExc(NewParams[i],CheckParamsType[i])
		continue
	    if NewParams[i].Type.Type == "fixed" and CheckParamsType[i].Type == "point":
		if NewParams[i].Type.Base.Id == CheckParamsType[i].Base.Id:
			NewParams[i] = BoxExc(NewParams[i],CheckParamsType[i])

	    if NewParams[i].Type.Type == "point" and NewParams[i].Type.Base.Type == "class" and NewParams[i].Type.Base.Id == CheckParamsType[i].Id:
			NewParams[i] = BoxRefCandy(NewParams[i])
	    if NewParams[i].Type.Id != CheckParamsType[i].Id:
		#print(len(NewParams[i].Type.Params))
		#for It in CheckParams[i].Params:
		#	print(It.Type.Id)
		raise ValueError("Type mismatch in function call " + self.ToCall)
        return None



def GetUse(Obj):
    if Obj.Value in ["d()","dm","db","dp","d.d()","d_in_d"]:
        return  BoxFuncsCall(Obj)
    if Obj.Value in ["id","this"]:
        return  BoxParamCall(Obj)
    if Obj.Value in ["ds"]:
	return GetUse(Obj.Extra[0].Extra[0])
    if Obj.Value in ["d.d"]:
        return  BoxMetodCall(Obj)
    if Obj.Value in ["d&"]: 
        return BoxRef(Obj)
    if Obj.Value in ["![]"]: 
        return ParArrConst(Obj)
    if Obj.Value in ["d^","d[]"]: 
        return BoxPoint(Obj)
    if Obj.Value in ["newconst"]:
	return BoxConst(Obj)
    if Obj.Value in ["newstuf"]:
	return BoxNew(Obj)
    if Obj.Value == "daif": 
        return BoxIf(Obj)
    if Obj.Value == "dafor": 
        return BoxFor(Obj)
    if Obj.Value == "dawhile": 
        return BoxWhile(Obj)
    if Obj.Value == "d->{}":
	return BoxExc(Obj)
    if Obj.Value == "str": 
        return ParConstStr(Obj)
    if Obj.Value in ["numf","numi","true","false","null"]:
        return ParConstNum(Obj)
    if Obj.Value == "classtemplate":
        return BoxTemplate(Obj)
    if Obj.Value == "newclass":
        return BoxClass(Obj)
    if Obj.Value == "ret":
        return BoxReturn(Obj)
    if Obj.Value == "newtype":
	return BoxNewType(Obj)
        AddFuncPoint(Obj.Extra[3],Obj.Extra[0].Extra)
        return None
    if Obj.Value == "d.{}":
	return None
    if Obj.Value == "d..d":
	return ParRangeNum(Obj)
	
    raise ValueError("Not implemented {} at line {} in file {}".format(Obj.Value,Obj.Line,Obj.InFile))

class BoxBlock:
    def __init__(self,List):
        value ="~box"
        self.Items = []
        
        GotSome = True
        self.Items = []
        for i in range(len(List)):
	    It = List[i]
            PreAdd = None
            if It.Value == "newparam":
                PreAdd = ParamChain(It)
            else:
                PreAdd = GetUse(It)
            if PreAdd != None:
                self.Items.append(PreAdd)

    def PrintConst(self,F):
        for It in self.Items:
            It.PrintConst(F)
    def PrintFunc(self,F):
        for It in self.Items:
            It.PrintFunc(F)
    def PrintUse(self,F):
        for It in self.Items:
            It.PrintInBlock(F)
    def PrintInBlock(self,F):
        for It in self.Items:
            It.PrintInBlock(F)
    def PrintAlloc(self,F):
	for It in self.Items:
		It.PrintAlloc(F)
    def MakeGlobal(self):
	for It in self.Items:
		if It.Value == "~:=":
			It.IsGlobal = True
    def PrintGlobal(self,F):
	for It in self.Items:
		if It.Value == "~:=":
			It.PrintInBlock(F)
    def Check(self):
        PushC()
	PushT()
	for It in self.Items:
		if It.Value == "~newtype":
			It.Check()
	for It in self.Items:
	    if It.Value == "~const":
		ConstTable[It.Name] = It
        for It in self.Items:
            if It.Value == "~:=" and It.Extra != None and  It.Extra.Value == "~Func":
                ContextStuff.append(It)
	    if It.Value in ["~class","~templ"]:
		AddClass(It)
            if It.Value == "~:=" and (It.Extra == None or It.Extra.Value != "~Func") and It.Value != "~newtype":
                ContextStuff.append(It)
	for It in self.Items:
            if It.Value == "~:=" and  It.Extra != None and It.Extra.Value == "~Func":
                for Par in It.Extra.Params:
			Par.Check()
        for It in self.Items:
            It.Check()
	PopT()
        PopC()

class BoxFunc:
    def __init__(self,Obj,Paren = None):
        self.Value = "~Func"
        self.Params = []
        self.Type = GetType("void")
        self.Block = None
        self.IsTemplate = False
        self.IsLocal = False
	self.Vargs = False
        self.AsmLine = None
	self.InClass = Paren
	self.RetRef = False
	self.PreType = None
	self.ReturnInInput = None
	self.IsChecked = False
	self.IsVirtual = False

	if Paren != None:
		self.Params.append(ParamChain(Paren,Token("id","this")))
	
        if Obj == None:
            return None
        
        i = 0
	if Obj.Extra[i].Value == "virtual":
		self.IsVirtual = True
		i += 1
	i += 1
        if Obj.Extra[i].Value == "()":
	    self.Line = Obj.Extra[i].Line
            Pars = Obj.Extra[i].Extra
            if len(Pars) > 0:
                Pars.append(Token(',',','))
                PArrs = []
                j = 0
                while j < len(Pars):
                    if Pars[j].Value == ',':
                        if len(PArrs) == 1:
                            if PArrs[0].Value == "...":
                                self.Params.append(ParamChain(GetType("..."),"~no"))
				self.Vargs = True
                            else: 
                                self.Params.append(ParamChain(None,PArrs[0]))
                        elif len(PArrs) == 2:
			    PreAdd = ParamChain(None,PArrs[1])
			    PreAdd.PreExtra = PArrs[0]
                            self.Params.append(PreAdd)
                        #else == 4   int x = 4
                        PArrs = []
                    else:
                        PArrs.append(Pars[j])
                    j += 1
            for c in self.Params:
                c.IsRef = False
	    if Paren != None:
		self.Params[0].IsRef = True
        i += 1
	if Obj.Extra[i].Value == "->":
		i += 1
		if Obj.Extra[i].Value == "ref":
			self.RetRef = True
			i += 1
		self.Type = ParseType(Obj.Extra[i])
		if self.Type == None:
			self.PreType = Obj.Extra[i]
		i += 1
        if Obj.Extra[i].Value != "declare":
            self.Block = BoxBlock(Obj.Extra[i].Extra)
    def PrintConst(self,F):
        if self.Block != None:
            self.Block.PrintConst(F)
    def PrintUse(self,F,PId,NewParams):
	for i in range(len(NewParams)):
		if i < len(self.Params) and self.Params[i].IsRef:
        		NewParams[i].PrintPointPre(F)
		else:
        		NewParams[i].PrintPre(F)
	if self.AsmLine == None: 
        	self.PrevId = GetNumb()
		if self.Type.Id != GetType("void").Id:
			F.write("%Tmp{} = ".format(PId))
		F.write("call ")
		self.PrintFType(F)
		self.PrintName(F)
		if self.ReturnInInput != None:
			Pars = [self.ReturnInInput] + self.Params
		else:
			Pars = self.Params
		F.write("(")
		for i in range(len(NewParams)):
			if i > 0:
				F.write(",")
			if i < len(Pars) and Pars[i].IsRef:
        			NewParams[i].PrintPointUse(F)
			else:
        			NewParams[i].PrintUse(F)
		F.write(")\n")
		
	else:
		if len(NewParams) == 1:
			if self.Params[0].IsRef:
				F.write(self.AsmLine.format("%Tmp{}".format(self.PrevId),NewParams[0].GetPName()))
			else:
				F.write(self.AsmLine.format("%Tmp{}".format(self.PrevId),NewParams[0].GetName()))
		if len(NewParams) == 2:
			if self.Params[0].IsRef:
				F.write(self.AsmLine.format("%Tmp{}".format(self.PrevId),NewParams[0].GetPName(),NewParams[1].GetName()))
			else:
				F.write(self.AsmLine.format("%Tmp{}".format(self.PrevId),NewParams[0].GetName(),NewParams[1].GetName()))
        return None
    def PrintFunc(self,F):
        if self.IsTemplate or self.Name == "for":
            return None
        if self.Block != None:
            self.Block.PrintFunc(F)
        if self.Block == None:
            F.write("declare ")
        else:
            F.write("define ")
	if self.ReturnInInput:
		F.write("void ")
	else:
		if self.RetRef:
			GetPoint(self.Type).PrintUse(F)
        	else:
			self.Type.PrintUse(F)
        F.write(" @")
        F.write(self.FullName)
        F.write("(")

	if self.ReturnInInput != None:
		NewPars = [self.ReturnInInput] + self.Params
	else:
		NewPars = self.Params
	
        if len(NewPars) > 0:
            for i in range(len(NewPars)):
                if i > 0:
			F.write(",")
                NewPars[i].PrintParam(F)
        F.write(")" + "; {} {}\n".format(self.Name,self.Line))
        if self.Block == None:
            return None
        F.write("{\n")
	self.Block.PrintAlloc(F)
	if self.InClass != None:
		self.InClass.UseFakes(F,self.Params[0])
        self.Block.PrintUse(F)
        if self.Type.Type == "standart":
            if self.Type.Base == "void":
                F.write("ret void\n")
	if self.ReturnInInput != None:
		F.write("ret void\n")
        F.write("}\n")
    def PrintName(self,F):
        F.write("@")
        F.write(self.FullName)
    def PrintPre(self,F):
        return None
    def PrintFPoint(self,F):
	self.PrintFType(F)
        F.write("*")
    def Check(self):
	if self.IsChecked:
		return None
	self.IsChecked = True

        PushC()
	InFunc.append(self)
	if self.Block != None and self.Name != "main":
		self.FullName = "func{}".format(GetNumb())
	else:
		self.FullName = self.Name
	
        for It in self.Params:
	    It.Check()
            ContextStuff.append(It)
	if self.PreType != None:
		self.Type = ParseType(self.PreType)
	if self.Type == None:
		raise ValueError("return type not found")
	if self.Type.Type == "class" and not self.RetRef:
		self.ReturnInInput = ParamChain(self.Type,Token("id","ret"))
		self.ReturnInInput.IsRef = True
		ContextStuff.append(self.ReturnInInput)
	
        if self.Block != None:
            self.Block.Check()
	InFunc.pop()
        PopC()
    def PrintFType(self,F):
	self.PrintType(F)
    def PrintType(self,F):
	if self.ReturnInInput != None:
		F.write("void")
		NewPars = [self.ReturnInInput] + self.Params
	else:
		NewPars = self.Params
		if self.RetRef:
        		GetPoint(self.Type).PrintUse(F)
		else:
        		self.Type.PrintUse(F)
	
        F.write("(")
        if len(NewPars) > 0:
            for i in range(len(NewPars)):
                if i > 0:
			F.write(",")
		if NewPars[i].IsRef:
                	GetPoint(NewPars[i].Type).PrintUse(F)
		else:
                	NewPars[i].Type.PrintUse(F)
        F.write(")")
        return None
    def GetFuncPointType(self):
	return GetFuncPoint(self.Params,self.Type)



MFunc = ["add","sub","mul","div","rem"]
QFunc = ["+","-","*","/","%"]

MCmp = ["eq","ne","gt","ge","lt","le"]
QCmp = ["==","!=",">",">=","<","<="]

for j in ["float","double"]:
	TestAdd = BoxFunc(None)
	TestAdd.AsmLine ="store " +j+ " {2} ," +j+ "* {1}\n"
	TestAdd.AsmLine +="{0} = fadd " +j+ " {2},0.0\n"
	TestAdd.Name = "="
	TestAdd.Type = GetType(j)
	TestAdd.Params.append(ParamChain(GetType(j),"~no"))
	TestAdd.Params[0].IsRef = True
	TestAdd.Params.append(ParamChain(GetType(j),"~no"))
	StandartStuff.append(TestAdd)

	TestAdd = BoxFunc(None)
	TestAdd.AsmLine ="{0} = fsub " +j+ " 0.0,{1}\n"
	TestAdd.Name = "-"
	TestAdd.Type = GetType(j)
	TestAdd.Params.append(ParamChain(GetType(j),"~no"))
	StandartStuff.append(TestAdd)
	
	for IFunc in range(len(MFunc)):
		TestAdd = BoxFunc(None)
		TestAdd.AsmLine ="{0} = f"
		TestAdd.AsmLine += MFunc[IFunc]
		TestAdd.AsmLine += " "
		TestAdd.AsmLine += j
		TestAdd.AsmLine +=" {1} , {2}\n"
		TestAdd.Name = QFunc[IFunc]
		TestAdd.Type = GetType(j)
		TestAdd.Params.append(ParamChain(GetType(j),"~no"))
		TestAdd.Params.append(ParamChain(GetType(j),"~no"))
		StandartStuff.append(TestAdd)

		TestAdd = BoxFunc(None)
		TestAdd.AsmLine ="{0}Pre = load " +j+ " , " +j+ "* {1}\n"
		TestAdd.AsmLine += "{0} = f" + MFunc[IFunc] + " " + j + " {0}Pre,{2}\n"
		TestAdd.AsmLine += "store " +j+ " {0} ," +j+ "* {1}\n"
		TestAdd.Name =  QFunc[IFunc] + "="
		TestAdd.Type = GetType(j)
		TestAdd.Params.append(ParamChain(GetType(j),"~no"))
		TestAdd.Params[0].IsRef = True
		TestAdd.Params.append(ParamChain(GetType(j),"~no"))
		StandartStuff.append(TestAdd)
	for ICmp in range(len(MCmp)):
		TestAdd = BoxFunc(None)
		TestAdd.AsmLine ="{0} = fcmp "
		TestAdd.AsmLine += " u" + MCmp[ICmp] + " "
		TestAdd.AsmLine += j
		TestAdd.AsmLine +=" {1} , {2}\n"
		TestAdd.Name = QCmp[ICmp]
		TestAdd.Type = GetType("bool")
		TestAdd.Params.append(ParamChain(GetType(j),"~no"))
		TestAdd.Params.append(ParamChain(GetType(j),"~no"))
		StandartStuff.append(TestAdd)
		

for j in ["s","u"]:
	for i in [8,16,32,64]:
		Si =  "{}".format(i)
		SI = "i" + Si
		CurType = GetType("{}{}".format(j,i))

		TestAdd = BoxFunc(None)
		TestAdd.AsmLine ="store " +SI+ " {2} ," +SI+ "* {1}\n"
		TestAdd.AsmLine +="{0} = add " +SI+ " {2},0\n"
		TestAdd.Name = "="
		TestAdd.Type = CurType
		TestAdd.Params.append(ParamChain(CurType,"~no"))
		TestAdd.Params[0].IsRef = True
		TestAdd.Params.append(ParamChain(CurType,"~no"))
		StandartStuff.append(TestAdd)

		TestAdd = BoxFunc(None)
		TestAdd.AsmLine ="{0} = sub " +SI+ " 0,{1}\n"
		TestAdd.Name = "-"
		TestAdd.Type = CurType
		TestAdd.Params.append(ParamChain(CurType,"~no"))
		StandartStuff.append(TestAdd)

		for IFunc in range(len(MFunc)):
			TestAdd = BoxFunc(None)
			TestAdd.AsmLine ="{0} = " 
			if IFunc in [3,4]:
				TestAdd.AsmLine += j
			TestAdd.AsmLine += MFunc[IFunc]
			TestAdd.AsmLine += " i" + Si +" {1} , {2}\n"
			TestAdd.Name = QFunc[IFunc] 
			TestAdd.Type = CurType
			TestAdd.Params.append(ParamChain(CurType,"~no"))
			TestAdd.Params.append(ParamChain(CurType,"~no"))
			StandartStuff.append(TestAdd)

			TestAdd = BoxFunc(None)
			TestAdd.AsmLine ="{0}Pre = load " +SI+ " , " +SI+ "* {1}\n"
			TestAdd.AsmLine += "{0} = "
			if IFunc in [3,4]:
				TestAdd.AsmLine += j
			TestAdd.AsmLine += MFunc[IFunc] + " " + SI + " {0}Pre,{2}\n"
			TestAdd.AsmLine += "store " +SI+ " {0} ," +SI+ "* {1}\n"
			TestAdd.Name =  QFunc[IFunc] + "="
			TestAdd.Type = CurType
			TestAdd.Params.append(ParamChain(CurType,"~no"))
			TestAdd.Params[0].IsRef = True
			TestAdd.Params.append(ParamChain(CurType,"~no"))
			StandartStuff.append(TestAdd)

		for ICmp in range(len(MCmp)):
			TestAdd = BoxFunc(None)
			TestAdd.AsmLine ="{0} = icmp "
			if ICmp in [2,3,4,5]:
				TestAdd.AsmLine += j
			TestAdd.AsmLine += MCmp[ICmp]
			TestAdd.AsmLine +=" i{}".format(i)
			TestAdd.AsmLine +=" {1} , {2}\n"
			TestAdd.Name = QCmp[ICmp]
			TestAdd.Type = GetType("bool")
			TestAdd.Params.append(ParamChain(CurType,"~no"))
			TestAdd.Params.append(ParamChain(CurType,"~no"))
			StandartStuff.append(TestAdd)

TestAdd = BoxFunc(None)
TestAdd.AsmLine ="{0} = xor i1 {1} , 1\n"
TestAdd.Name = "not"
TestAdd.Type = GetType("bool")
TestAdd.Params.append(ParamChain(GetType("bool"),"~no"))
StandartStuff.append(TestAdd)

TestAdd = BoxFunc(None)
TestAdd.AsmLine ="store i8* {2} ,i8** {1}\n"
TestAdd.Name = "="
TestAdd.Type = GetPoint(GetType("void"))
TestAdd.Params.append(ParamChain(GetPoint(GetType("void")),"~no"))
TestAdd.Params[0].IsRef = True
TestAdd.Params.append(ParamChain(GetPoint(GetType("void")),"~no"))
StandartStuff.append(TestAdd)

TestAdd = BoxFunc(None)
TestAdd.AsmLine ="{0} = icmp ne i8* {1} , {2}\n"
TestAdd.Name = "!="
TestAdd.Type = GetType("bool")
TestAdd.Params.append(ParamChain(GetPoint(GetType("void")),"~no"))
TestAdd.Params.append(ParamChain(GetPoint(GetType("void")),"~no"))
StandartStuff.append(TestAdd)

TestAdd = BoxFunc(None)
TestAdd.AsmLine ="{0} = icmp eq i8* {1} , {2}\n"
TestAdd.Name = "=="
TestAdd.Type = GetType("bool")
TestAdd.Params.append(ParamChain(GetPoint(GetType("void")),"~no"))
TestAdd.Params.append(ParamChain(GetPoint(GetType("void")),"~no"))
StandartStuff.append(TestAdd)

TestAdd = BoxFunc(None)
TestAdd.AsmLine ="{0} = and i1 {1} , {2}\n"
TestAdd.Name = "and"
TestAdd.Type = GetType("bool")
TestAdd.Params.append(ParamChain(GetType("bool"),"~no"))
TestAdd.Params.append(ParamChain(GetType("bool"),"~no"))
StandartStuff.append(TestAdd)

TestAdd = BoxFunc(None)
TestAdd.AsmLine ="{0} = or i1 {1} , {2}\n"
TestAdd.Name = "or"
TestAdd.Type = GetType("bool")
TestAdd.Params.append(ParamChain(GetType("bool"),"~no"))
TestAdd.Params.append(ParamChain(GetType("bool"),"~no"))
StandartStuff.append(TestAdd)

TestAdd = BoxFunc(None)
TestAdd.AsmLine ="store i1 {2} ,i1* {1}\n"
TestAdd.Name = "="
TestAdd.Type = GetType("void")
TestAdd.Params.append(ParamChain(GetType("bool"),"~no"))
TestAdd.Params[0].IsRef = True
TestAdd.Params.append(ParamChain(GetType("bool"),"~no"))
StandartStuff.append(TestAdd)

TestAdd = BoxFunc(None)
TestAdd.AsmLine ="{0} = icmp eq i1 {2} , {1}\n"
TestAdd.Name = "=="
TestAdd.Type = GetType("bool")
TestAdd.Params.append(ParamChain(GetType("bool"),"~no"))
TestAdd.Params.append(ParamChain(GetType("bool"),"~no"))
StandartStuff.append(TestAdd)

TestAdd = BoxFunc(None)
TestAdd.AsmLine  ="{0}Div = udiv i32 {1},32\n"
TestAdd.AsmLine +="{0}Rem = urem i32 {1},32\n"
TestAdd.AsmLine +="{0}Point = getelementptr [8 x i32],[8 x i32]* {2},i32 0,  i32 {0}Div\n"
TestAdd.AsmLine +="{0}One = shl i32 1 , {0}Rem\n"
TestAdd.AsmLine +="{0}Value = load i32,i32* {0}Point\n"
TestAdd.AsmLine +="{0}And = and i32 {0}Value , {0}One\n" 
TestAdd.AsmLine +="{0} = icmp ne i32 {0}And , 0\n"
TestAdd.Name = "in"
TestAdd.Type = GetType("bool")
TestAdd.Params.append(ParamChain(GetType("int"),"~no"))
TestAdd.Params.append(ParamChain(GetFixedArr(GetType("u32"),32),"~no"))
TestAdd.Params[1].IsRef = True
StandartStuff.append(TestAdd)
