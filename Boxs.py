from Lex import *
from MathS import *
from allnums import *
from Type import *
import struct

ContextStuff = []
ContextName = []
NameStack = []
StuffStack = []
ConstTable = {}

StandartStuff = []

Opers = ["=","+","-","*","/","%","+=","-=","*=","/=","%="]

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

def GoodPoints(Par1,Par2):
    if Par1.Id == GetVoidP() and Par2.IsPoint and not Par2.Base.IsPoint:
       return True
    if Par2.Id == GetVoidP() and Par1.IsPoint and not Par1.Base.IsPoint:
       return True
    return False
    
    
def GetFunc(Name,Pars):
	PreRet = SearchFunc(Name,Pars,ContextStuff)
	if PreRet != None:
		return PreRet
	return SearchFunc(Name,Pars,StandartStuff)

def SearchFunc(Name,Pars,Arr):
    WrongValue = False
    for i in reversed(range(len(Arr))):
	Itc = Arr[i]
        if Itc.Name == Name:
	    if Itc.Value == "~:=":
            	It = Itc.Extra
	    else:
		It = Itc
            WrongValue = False
	    if len(Pars) != len(It.Params) and Name != "printf":
		continue
            for i in range(len(It.Params)):
		It.Params[i].Check()
		if It.Params[i].Type == None:
			print(It.Name)
                if It.Params[i].Type.Id == GetType("...").Id:
                    return It
		if i >= len(Pars):
			WrongValue = True
			break
		if Pars[i].Type.Type == "class" and It.Params[i].Type.Type == "class" and Pars[i].Type.InFamily(It.Params[i].Type):
			continue
                if GoodPoints(It.Params[i].Type,Pars[i].Type):
                    continue
                if It.Params[i].Type.Type == "funcp":
                    continue
                if It.Params[i].Type.Id != Pars[i].Type.Id:
                    #print("wrong param {} {} {} {}".format(It.Name,i,It.Params[i].Type.Id,Pars[i].Type.Id))
                    WrongValue = True
                    break
            if not WrongValue:
                return It
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
	def PrintPre(self,F):
		return None
	def PrintFunc(self,F):
		return None
	def PrintConst(self,F):
		return None
	def Check(self):
		return None

def GetParam(Name):
    WrongValue = False
    for i in reversed(range(len(ContextStuff))):#ContextStuff:
	It = ContextStuff[i]
        if It.Name == Name:
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
	self.ParParam = None
	self.IndParam = None
	self.IsFixed = -1
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
	if self.IsNumI:
		self.ParParam.PrintPointPre(F)
		F.write("store i32 0, i32* {}\n".format(self.ParParam.GetPName()))
        	F.write("br label %ForCheck{}\n".format(self.Id))
        	F.write("ForCheck{}:\n".format(self.Id))
        	self.Quest.PrintPre(F)
		self.ParParam.PrintPre(F)
		F.write("%Result{} = icmp slt i32 {} , {}\n".format(self.Id,self.ParParam.GetName(),self.Quest.GetName()))
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
		
        #F.write("br label %ForCheck{}\n".format(self.Id))
        #F.write("ForCheck{}:\n".format(self.Id))
        #self.Quest.PrintPre(F)
        #F.write("br ")
        #self.Quest.PrintUse(F)
        #F.write(", label %WhileT{0}, label %WhileF{0}\n".format(self.Id))

        #F.write("WhileStart{}:\n".format(self.Id))
        #self.Job.PrintInBlock(F)
        #F.write("br label %ForCheck{}\n".format(self.Id))
        #F.write("ForEnd{}:\n".format(self.Id))
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
	if self.Quest.Type.Id == GetType("int").Id:
		self.IsNumI = True
		self.ParParam = ParamChain(GetType("int"),Token("id",self.ParName))
	elif self.Quest.Type.Type == "fixed":
		self.IsFixed = self.Quest.Type.Size
		self.ParParam = ParamChain(self.Quest.Type.Base,Token("id",self.ParName))
		self.ParParam.IsDrived = True
		self.IndParam = ParamChain(GetType("int"),Token("id",self.IndName))
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
        if self.Quest.Type != GetType("bool").Id:
            self.Quest = BoxExc(self.Quest,GetType("bool"))
        
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

        if NotObj == None:
            self.Name = Obj.Extra[0].Extra
            self.Extra = None
            self.Type = None

            if Obj.Extra[2].Value == "!()":
                self.IsFunc = True
                self.Extra = BoxFunc(Obj.Extra[2])
                self.Extra.Name = self.Name
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
	if self.PreExtra != None:
		self.Type = ParseType(self.PreExtra)
		if self.Type == None:
			self.Extra = GetUse(self.PreExtra)
			if self.Extra == None:
				raise ValueError("Unknown Object")
        if self.Extra != None:
            self.Extra.Check()
            self.Type = self.Extra.Type
	if self.Type.Type == "class":
		self.IsRef = True
	self.IsChecked = True
    def GetPName(self):
	if self.IsGlobal:
		return "@Tmp{}".format(self.Id)
	else:
		return "%Tmp{}".format(self.Id)
    def GetName(self):
	if self.Type.Type == "fixed":
		return self.GetPName()
	if self.IsGlobal:
		# TODO: ref or const
        	return "@Tmp{}".format(self.Id)
	else:
		if self.IsRef:
        		return "%Tmp{}".format(self.PrevId)
		else:
			return "%Tmp{}".format(self.Id)
    def GetPoint(self):
	if self.IsGlobal:
        	return "@Tmp{}".format(self.Id)
	else:
        	return "%Tmp{}".format(self.Id)
    def PrintAlloc(self,F):
	if self.IsGlobal:
		raise ValueError("Impossible error")
	if self.IsDrived:
		return None
	if not self.IsFunc:
		if self.Type.Type == "fixed":
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
    def PrintPointUse(self,F):
	GetPoint(self.Type).PrintUse(F)
	F.write(" %Tmp{}".format(self.Id))
	return None
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
			F.write("@Tmp{} = global ".format(self.Id))
			self.Type.PrintZero(F)
			F.write("\n")
		return None
        if not self.IsFunc and self.Type != None:
            if self.Extra != None:
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
		F.write("%TmpPre{0} = call i8* @malloc(i32 %EndSize{0})\n".format(self.PrevId))
		F.write("%Tmp{0} = bitcast i8* %TmpPre{0} to ".format(self.PrevId))
		self.Type.PrintUse(F)
		F.write("\n")
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
	def Check(self):
		if self.Extra != None:
			self.Extra.Check()
		for it in self.Pars:
			it.Check()
		self.Type = ParseType(self.PreType)
		if self.Type == None:
			raise ValueError("Not found type")
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
			self.ToUse = self.Type.GetFunc("this",self.Pars)
			if self.ToUse == None:
				raise ValueError("Constructor not found")
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
		if UseId == -1:
			UseId = self.Id
		if ToUse.Type.IsPoint:
			ToUse.PrintPre(F)
		else:
			ToUse.PrintPointPre(F)
		F.write("%Tmp{} = getelementptr ".format(UseId))
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
			PrePre = UseId
			UseId = GetNumb()
			F.write("%Tmp{} = bitcast ".format(UseId))
			if self.Type.Type == "fixed":
				if self.PrevType == None:
					self.Type.PrintInAlloc(F)
				else:
					self.PrevType.PrintInAlloc(F)
				F.write("*")
			else:
				GetPoint(self.PrevType).PrintUse(F)
			F.write(" %Tmp{}".format(PrePre))
			F.write(" to ")
			GetPoint(self.Type).PrintUse(F)
			F.write("\n")
	def GetName(self):
		return "%Tmp{}".format(self.PrevId)
	def GetPName(self,UseId = -1):
		if UseId == -1:
			UseId = self.Id
		return "%Tmp{}".format(UseId)
	def PrintPointPre(self,F,UseId = -1):
		return None
	def PrintPointUse(self,F,UseId = -1):
		if UseId == -1:
			UseId = self.Id
		GetPoint(self.Type).PrintUse(F)
		F.write(" %Tmp{}".format(UseId))
		return None
	def PrintUse(self,F,UseId = -1):
		if UseId == -1:
			UseId = self.Id
		if self.Type.Type == "fixed":
			self.PrintPointUse(F,UseId)
			return None
		self.Type.PrintUse(F)
		F.write(" %Tmp{}".format(self.PrevId))
	def PrintPre(self,F,UseId = -1):
		if UseId == -1:
			UseId = self.Id
		if self.Type.Type == "fixed":
			self.PrintPointPre(F)
			return None
		self.PrevId = GetNumb()
		F.write("%Tmp{} = load ".format(self.PrevId))
		self.Type.PrintUse(F)
		F.write(",")
		GetPoint(self.Type).PrintUse(F)
		F.write(" %Tmp{}\n".format(UseId))
	def Check(self,Paren = None):
		if Paren == None:
			return None
		self.InClass = Paren
		self.Pos = Paren.GetPos(self.ToCall)
		if self.Pos < 0:
			raise ValueError("There is no object {}".format(self.ToCall))
		if self.PreType == None:
			self.Type = Paren.Items[self.Pos].Type 
		else:
			self.PrevType = Paren.Items[self.Pos].Type
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
					
	def Check(self):
		if self.ToExtend != None:
			self.ToExtend = ParseType(self.ToExtend)
			if self.ToExtend == None or self.ToExtend.Value != "~class":
				raise ValueError("Nothing to extend")
			self.Items = self.Items + self.ToExtend.Items 
			self.Fakes = self.Fakes + self.ToExtend.Fakes
		for It in self.Items:
			It.Check()
		for It in self.Fakes:
			It.Check(self)
		for It in self.Fakes:
			ContextStuff.append(It)
		for It in self.Funcs:
			It.Check()
	def PrintInAlloc(self,F):
		F.write("%Class{}".format(self.Id))
	def PrintConst(self,F):
		for It in self.Items:
			It.PrintConst(F)
		for It in self.Funcs:
			It.PrintConst(F)
		F.write("%Class{} = type {}".format(self.Id,"{"))
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
		SomeFunc =  SearchFunc(Res,Pars,self.Funcs)
		if SomeFunc != None:
			return SomeFunc
		if self.ToExtend != None:
			return self.ToExtend.GetFunc(Res,Pars)
		return None
		
	def GetPos(self,Res):
		for i in range(len(self.Items)):
			if self.Items[i].Name == Res:
				return i
		return -1
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
        while i < len(Obj.Extra):
            if Obj.Extra[self.Size] == '\\':
                i += 1
            self.Size += 1
            i += 1
        self.Size += 1
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
        return "%Lstr{}".format(self.Id)
    def PrintUse(self,F):
        F.write("i8* %LStr{}".format(self.Id))
    def Check(self):
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
    def PrintConst(self,F):
        return None
    def Check(self):
        return None
    def PrintConst(self,F):
        if self.Extra != None and hasattr(self.Extra,"PrintConst"):
            self.Extra.PrintConst(F)
        return None
    def PrintFunc(self,F):
        return None
    def PrintAsConst(self,F):
	if self.Type.Id == GetType("float").Id:
        	F.write(" float {}\n".format(hex(struct.unpack("<q",struct.pack("<d",self.Extra))[0])))
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
            F.write("%Tmp{} = fptrunc double {} to float\n".format(self.PrevId,hex(struct.unpack("<q",struct.pack("<d",self.Extra))[0])))

    def PrintUse(self,F):
	if self.Type.Id == GetVoidP():
		self.Type.PrintUse(F)
		F.write(" null")
	else:
        	self.Type.PrintUse(F)
        	F.write(" %Tmp{}".format(self.PrevId))
        return None
class BoxReturn:
    def __init__(self,Obj):
        self.Value = "~ret"
        self.Ret = GetUse(Obj.Extra[1])
    def PrintConst(self,F):
        return None
    def PrintFunc(self,F):
        return None
    def Check(self):
        if self.Ret != None:
            self.Ret.Check()
    def PrintPre(self,F):
        return None
    def PrintAlloc(self,F):
	return None
    def PrintUse(self,F):
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
	self.ToUse.PrintInBlock(F,self.Param,self.PrevId)
	self.ToUse.PrintPre(F,self.PrevId)
    def GetPName(self):
	return self.ToUse.GetPName(self.PrevId)
        #return "%Tmp{}".format(self.Id)
    def GetName(self):
	return self.ToUse.GetName()
    def PrintUse(self,F):
	self.ToUse.PrintUse(F,self.PrevId)
    def PrintInBlock(self,F):
        return None
    def PrintAlloc(self,F):
	return None
    def Check(self):
	self.Param.Check()
	#self.Object = self.Param.Object
        self.ClassType = self.Param.Type
	if self.ClassType.Type != "class":
		if self.ClassType.Base.Type != "class":
			raise ValueError("Not a class")
		else: 
			self.UseClass = self.ClassType.Base
	else:
		self.UseClass = self.ClassType
	self.ToUse = self.UseClass.GetFake(self.ToCall)
	if self.ToUse == None:
		raise ValueError("Object does not have field {}".format(self.ToCall))
	self.Type = self.ToUse.Type
    def PrintPointPre(self,F):
	self.PrevId = GetNumb()
	self.ToUse.PrintInBlock(F,self.Param,self.PrevId)
	self.ToUse.PrintPointPre(F,self.PrevId)
    def PrintPointUse(self,F):
	self.ToUse.PrintPointUse(F,self.PrevId)
class BoxParamCall:
    def __init__(self,Obj):
        self.Value = "~id"
        self.Type = GetType("void")
        self.PrevId = None
        self.Object = None
        self.ToUse = Obj.Extra
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
    def PrintInBlock(self,F):
        return None
    def Check(self):
        self.Object = GetParam(self.ToUse)
        if self.Object == None:
            raise ValueError("Object not found {}".format(self.ToUse))
	self.Object.Check()
        self.Type = self.Object.Type
        self.Id = self.Object.Id
        #self.Type = self.CallFunc.RetType
        return None
    def PrintPointPre(self,F):
        #Tempt TEMP
        return None
    def PrintPointUse(self,F):
        GetPoint(self.Type).PrintUse(F)
	if self.Object.IsGlobal:
        	F.write(" @Tmp{}".format(self.Id))
	else:
        	F.write(" %Tmp{}".format(self.Id))
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
	

class BoxPoint:
    def __init__(self,Obj):
        if Obj.Value == "d^":
            self.Value = "~d^"
            self.Extra = GetUse(Obj.Extra[0])
        if Obj.Value == "d[]":
            self.Value = "~d[]"
            self.Extra = GetUse(Obj.Extra[0])
            self.Ind = GetUse(Obj.Extra[1].Extra[0])
    def PrintPointPre(self,F):
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
        if self.Value == "~d[]":
            self.Extra.Type.PrintUse(F)
            F.write(" %Tmp{}".format(self.PrevId))
        else:
            self.Extra.PrintUse(F)
    def Check(self):
        self.Extra.Check()
		
        if not self.Extra.Type.IsPoint:
            	raise ValueError('Not a point')
        self.Type = self.Extra.Type.Base

        if self.Value == "~d[]":
            self.Ind.Check()
    def GetName(self):
        return "%Tmp{}".format(self.PrevId)
    def GetPName(self):
        if self.Value == "~d[]":
            return "%Tmp{}".format(self.PrevId)
        else:
            self.Extra.GetName(F)
    def PrintFunc(self,F):
        self.Extra.PrintFunc(F)
        if self.Value == "~d[]":
            self.Ind.PrintFunc(F)
    def PrintConst(self,F):
        self.Extra.PrintConst(F)
        if self.Value == "~d[]":
            self.Ind.PrintConst(F)
    def PrintPre(self,F):
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
        self.Type.PrintUse(F)
        F.write(" %Tmp{}".format(self.PrevId))

class BoxExc:
    def __init__(self,Obj,NewType):
        self.Extra = Obj
        self.Value = "~[T]"
        self.Type = NewType
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
        self.Extra.PrintConst(F)
    def Check(self):
	self.Extra.Check()

class BoxFuncsCall:
    def __init__(self,Obj):
        self.Value = "~d()"
        self.Params = []
        self.Type = GetType("void")
        self.PrevId = None
        self.CallFunc = None
	self.IsMetod = False
        
        
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
            if len(Pars) != 0:
                i = 0
                while i < len(Pars):
                    if Pars[i].Value != ',':
                        self.Params.append(GetUse(Pars[i]))                    
                    i += 1
	else:
            self.ToCall = Obj.Extra[1].Value
            #print(Obj.Extra[0].Value)
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
	return None
    def PrintPre(self,F):
        self.PrevId = GetNumb()
	for i in range(len(self.Params)):
		if i < len(self.CallFunc.Params) and self.CallFunc.Params[i].IsRef:
        		self.Params[i].PrintPointPre(F)
		else:
        		self.Params[i].PrintPre(F)
	if self.CallFunc.AsmLine == None: 
        	self.PrevId = GetNumb()
		
		if self.CallFunc.Type.Id != GetType("void").Id:
			F.write("%Tmp{} = ".format(self.PrevId))
		F.write("call ")
		self.CallFunc.PrintFType(F)
		self.CallFunc.PrintName(F)
		F.write("(")
		for i in range(len(self.Params)):
			if i > 0:
				F.write(",")
			if i < len(self.CallFunc.Params) and self.CallFunc.Params[i].IsRef:
        			self.Params[i].PrintPointUse(F)
			else:
        			self.Params[i].PrintUse(F)
		F.write(")\n")
		
	else:
		if len(self.Params) == 1:
			if self.CallFunc.Params[0].IsRef:
				F.write(self.CallFunc.AsmLine.format("%Tmp{}".format(self.PrevId),self.Params[0].GetPName()))
			else:
				F.write(self.CallFunc.AsmLine.format("%Tmp{}".format(self.PrevId),self.Params[0].GetName()))
		if len(self.Params) == 2:
			if self.CallFunc.Params[0].IsRef:
				F.write(self.CallFunc.AsmLine.format("%Tmp{}".format(self.PrevId),self.Params[0].GetPName(),self.Params[1].GetName()))
			else:
				F.write(self.CallFunc.AsmLine.format("%Tmp{}".format(self.PrevId),self.Params[0].GetName(),self.Params[1].GetName()))
        return None
    def GetName(self):
        return "%Tmp{}".format(self.PrevId)
    def PrintUse(self,F):
        self.Type.PrintUse(F)
        F.write(" %Tmp{}".format(self.PrevId))
        return None
    def PrintInBlock(self,F):
        self.PrintPre(F)
    def Check(self):
        for It in self.Params:
            It.Check()
	
	if len(self.Params) > 0 and self.Params[0].Type.Type == "class":
        	self.CallFunc = self.Params[0].Type.GetFunc(self.ToCall,self.Params)
	else:
        	self.CallFunc = GetFunc(self.ToCall,self.Params)
        if self.CallFunc == None:
		PreError = "Func not found {}".format(self.ToCall)
		for it in self.Params:
			PreError += "\nParam {}".format(it.Type.GetName())
            	raise ValueError(PreError)
        self.Ret = self.CallFunc
        self.Type = self.CallFunc.Type

	i = 0
        for i in range(len(self.Params)):
	    if self.CallFunc.Params[i].Type.Id == self.Params[i].Type.Id:
		continue
            if self.CallFunc.Params[i].Type == GetType("..."):
		while i < len(self.Params):
			if self.Params[i].Type.Id == GetType("float").Id:
				self.Params[i] = BoxExc(self.Params[i],GetType("double"))
			i += 1
            	break
            if self.CallFunc.Params[i].Type.Id != self.Params[i].Type.Id and GoodPoints(self.Params[i].Type,self.CallFunc.Params[i].Type):
                self.Params[i] = BoxExc(self.Params[i],self.CallFunc.Params[i].Type)
	    if self.Params[i].Type.Type == "class":
		self.Params[i] = BoxExc(self.Params[i],self.CallFunc.Params[i].Type)
        return None



def GetUse(Obj):
    if Obj.Value in ["d()","dm","db","dp","d.d()"]:
        return  BoxFuncsCall(Obj)
    if Obj.Value in ["id","this"]:
        return  BoxParamCall(Obj)
    if Obj.Value in ["ds"]:
	return GetUse(Obj.Extra[0].Extra[0])
    if Obj.Value in ["d.d"]:
        return  BoxMetodCall(Obj)
    if Obj.Value in ["d&"]: 
        return BoxRef(Obj)
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
        #VERY TEMP
        AddFuncPoint(Obj.Extra[3],Obj.Extra[0].Extra)
        return None
    if Obj.Value == "d.{}":
	return None
	
    raise ValueError("Not implemented {} at line {}".format(Obj.Value,Obj.Line))
    #print("Not implemented {}".format(Obj.Value))

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
            if It.Value == "~:=" and It.Extra != None and  It.Extra.Value == "~Func":
                ContextStuff.append(It)
	    if It.Value in ["~class","~templ"]:
		AddClass(It)
	    if It.Value == "~const":
		ConstTable[It.Name] = It
        for It in self.Items:
            It.Check()
            if It.Value == "~:=" and (It.Extra == None or It.Extra.Value != "~Func"):
                ContextStuff.append(It)
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

	if Paren != None:
		self.Params.append(ParamChain(Paren,Token("id","this")))
	
        if Obj == None:
            return None
        
        if Obj.Extra[1].Value == "()":
            Pars = Obj.Extra[1].Extra
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
        i = 3
        while Obj.Extra[i].Value not in  ["{}","declare"]:
            i += 1
        self.Type = ParseType(Obj.Extra[3]) # VERY TEMP
        i  = 4 # Temp
        if Obj.Extra[i].Value != "declare":
            self.Block = BoxBlock(Obj.Extra[i].Extra)
    def PrintConst(self,F):
        if self.Block != None:
            self.Block.PrintConst(F)
    def PrintUse(self,F,PId,ParList):
        if self.AsmLine == None:

            if self.Type.Id != 0:
                F.write("%Tmp{} = ".format(PId))
            F.write("call ")
            self.PrintType(F)
            F.write(" ")
            self.PrintName(F)
            F.write("(")
            if len(ParList) > 0:
                ParList[0].PrintUse(F)
                for i in range(1,len(ParList)):
                    F.write(",")
                    ParList[i].PrintUse(F)
            F.write(")\n")
        else:
            if len(ParList) == 2:
                F.write(self.AsmLine.format("Tmp{}".format(PId),ParList[0].GetName(),ParList[1].GetName()))
            else:
                F.write(self.AsmLine.format("Tmp{}".format(PId),ParList[0].GetName()))

    def PrintFunc(self,F):
        if self.IsTemplate:
            return None
        if self.Block != None:
            self.Block.PrintFunc(F)
        if self.Block == None:
            F.write("declare ")
        else:
            F.write("define ")
        self.Type.PrintUse(F)
        F.write(" @")
        F.write(self.FullName)
        F.write("(")
        if len(self.Params) > 0:
            for i in range(len(self.Params)):
                if i > 0:
			F.write(",")
                self.Params[i].PrintParam(F)
        F.write(")" + "; {}\n".format(self.Name))
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
        PushC()
	if self.Block != None and self.Name != "main":
		self.FullName = "func{}".format(GetNumb())
	else:
		self.FullName = self.Name
	
        for It in self.Params:
	    It.Check()
            ContextStuff.append(It)
        if self.Block != None:
            self.Block.Check()
        PopC()
    def PrintFType(self,F):
	self.PrintType(F)
    def PrintType(self,F):
        self.Type.PrintUse(F)
        F.write("(")
        if len(self.Params) > 0:
            for i in range(len(self.Params)):
                if i > 0:
			F.write(",")
		if self.Params[i].IsRef:
                	GetPoint(self.Params[i].Type).PrintUse(F)
		else:
                	self.Params[i].Type.PrintUse(F)
        F.write(")")
        return None



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

