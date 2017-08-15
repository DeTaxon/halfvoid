from Lex import *
from MathS import *
from allnums import *
from Type import *
import struct

ContextStuff = []
ContextName = []
NameStack = []
StuffStack = []

StandartStuff = []

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
    WrongValue = False
    for Itc in ContextStuff:
        if Itc.Name == Name:
            It = Itc.Extra
            WrongValue = False
            for i in range(len(It.Params)):
                if It.Params[i].Type.Id == GetType("...").Id:
                    return It
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
    for It in StandartStuff:
        if It.Name == Name:
            WrongValue = False
            for i in range(len(It.Params)):
                if It.Params[i].Type.Id == GetType("...").Id:
                    return It

                if It.Params[i].Type.Id != Pars[i].Type.Id:
                    #print("wrong param {} {} {} {}".format(It.Name,i,It.Params[i].Type.Id,Pars[i].Type.Id))
                    WrongValue = True
                    break
            if not WrongValue:
                return It
    return None

def GetParam(Name):
    WrongValue = False
    for It in ContextStuff:
        if It.Name == Name:
            return It
    return None

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
	self.IsGlobal = False

        if NotObj == None:
            self.Name = Obj.Extra[0].Extra
            self.Extra = None
            self.Type = None

            if Obj.Extra[2].Value == "!()":
                self.IsFunc = True
                self.Extra = BoxFunc(Obj.Extra[2])
                self.Extra.Name = self.Name
            else:
                self.Type = ParseType(Obj.Extra[2])
                if self.Type == None:
                    self.Extra = GetUse(Obj.Extra[2])
                    self.Type = self.Extra.Type
        else:
            self.Name = None
            if NotObj != "~no":
                self.Name = NotObj.Extra
            self.Extra = None
            self.Type =  Obj

    def PrintParam(self,F):
        if self.Type != None:
            self.Type.PrintUse(F)
        F.write(" ")
        if self.Name != None:
            F.write("%")
            F.write(self.Name)
    def PrintConst(self,F):
        if self.Extra != None:
            self.Extra.PrintConst(F)
        return None
    def PrintPre(self,F):
        if self.IsFunc:
            return None
        if self.IsRef:
            self.PrevId = GetNumb()
            F.write("%Tmp{} = load ".format(self.PrevId))
            self.Type.PrintUse(F)
            F.write(" , ")
            self.Type.PrintUse(F)
	    if self.IsGlobal:
            	F.write("* @Tmp{}\n".format(self.Id))
	    else:
            	F.write("* %Tmp{}\n".format(self.Id))
    def PrintUse(self,F):
        if self.IsFunc:
            self.Extra.PrintFPoint(F)
            F.write(" @{}".format(self.Name))
            return None
        if self.IsRef:
            self.Type.PrintUse(F)
            F.write(" %Tmp{}".format(self.PrevId))
        else:
            self.Type.PrintUse(F)
            F.write(" %{}".format(self.Name))
    def PrintFunc(self,F):
        if not self.IsFunc:
            return None
        self.Extra.PrintFunc(F)
    def Check(self):
        if self.Extra != None:
            self.Extra.Check()
            self.Type = self.Extra.Type
        return None
    def GetName(self,F):
	if self.IsGlobal:
        	return "@Tmp{}".format(self.PrevId)
	else:
        	return "%Tmp{}".format(self.PrevId)
    def GetPoint(self):
	if self.IsGlobal:
        	return "@Tmp{}".format(self.Id)
	else:
        	return "%Tmp{}".format(self.Id)
    def PrintInBlock(self,F):
	if self.IsGlobal and not self.IsFunc:
		F.write("@Tmp{} = global ".format(self.Id))
		if self.Extra == None:
			self.Type.PrintZero(F)
		F.write("\n")
		return None
        if not self.IsFunc and self.Type != None:
            if self.Extra != None:
                self.Extra.PrintPre(F)
            F.write("%Tmp{} = alloca ".format(self.Id))
            self.Type.PrintUse(F)
            F.write("\n")
            if self.Extra != None:
                F.write("store ")
                self.Extra.PrintUse(F)
                F.write(", ")
                self.Type.PrintUse(F)
                F.write("* %Tmp{}\n".format(self.Id))
            

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
        return "Lstr{}".format(self.Id)
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
            self.Type = GetType("float") #GetType("float")
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
    def GetName(self):
        return "Tmp{}".format(self.PrevId)
    def PrintPre(self,F):
        self.PrevId = GetNumb()
        if self.Type.Id == GetType("int").Id or self.Type.Id == GetType("bool").Id:
            F.write("%Tmp{} = add ".format(self.PrevId))
            self.Type.PrintUse(F)
            F.write(" {} , 0\n".format(self.Extra))
        else:
		#F.write("%Tmp{} = fadd float 0x{} , 0.0\n".format(self.PrevId,struct.pack(">F",self.Extra).encode("hex")))
		#F.write("%Tmp{} = fadd float {} , 0.0\n".format(self.PrevId,self.Extra.hex()))
            F.write("%Tmp{} = fptrunc double ".format(self.PrevId))
            F.write(" {}e+00 to  float\n".format(self.Extra))

    def PrintUse(self,F):
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
    def PrintUse(self,F):
        self.Ret.PrintPre(F)
        F.write("ret ")
        self.Ret.PrintUse(F)
        F.write("\n")
        return None
    def PrintInBlock(self,F):
        self.PrintUse(F)

class BoxParamCall:
    def __init__(self,Obj):
        self.Value = "~id"
        self.Type = GetType("void")
        self.PrevId = None
        self.Object = None
        self.ToUse = Obj.Extra
        
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
    def GetName(self):
        return "Tmp{}".format(self.PrevId)
    def PrintUse(self,F):
        #self.Ret.RetType.PrintUse(F)
        #F.write(" %Tmp{}".format(self.PrevId))
        #self.Object.Extra.PrintUse(F)
        self.Object.PrintUse(F)
        return None
    def PrintInBlock(self,F):
        return None
    def Check(self):
        self.Object = GetParam(self.ToUse)
        if self.Object == None:
            raise ValueError("Object not found {}".format(self.ToUse))
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
            F.write("%Tmp{} = icmp ne ".format(self.PrevId))
            self.Extra.PrintUse(F)
            F.write(" , 0\n")
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
    def PrintConst(self,F):
        self.Extra.PrintConst(F)
    def PrintFunc(self,F):
        self.Extra.PrintConst(F)
class BoxPrisvCall:
    def __init__(self,Obj):
        self.Value = "~dp"
        self.Params = []
        self.Type = GetType("void")
        self.PrevId = None
        self.CallFunc = None
        
        self.ToCall = Obj.Extra[1].Value
        self.Params.append(GetUse(Obj.Extra[0]))
        self.Params.append(GetUse(Obj.Extra[2]))
    def PrintConst(self,F):
        for P in self.Params:
            P.PrintConst(F)
    def PrintFunc(self,F):
        for P in self.Params:
            P.PrintFunc(F)
    def PrintPre(self,F):
        #for N in self.Params:
            #N.PrintPre(F)
        self.Params[1].PrintPre(F)
        self.PrevId = GetNumb()
        #self.CallFunc.PrintUse(F,self.PrevId,self.Params)
        self.Params[0].PrintPointPre(F)

        F.write("store ")
        self.Params[1].PrintUse(F)
        F.write(", ")
        self.Params[0].PrintPointUse(F)
        F.write("\n")
        return None
    def GetName(self):
        return "Tmp{}".format(self.PrevId)
    def PrintUse(self,F):
        return None
    def PrintInBlock(self,F):
        self.PrintPre(F)
        return None
    def Check(self):
        for It in self.Params:
            It.Check()

        self.Type = self.Params[0].Type
        if self.Params[0].Type.Id != self.Params[1].Type.Id:
        	if self.Params[1].Type.Id != self.Type.Id and GoodPoints(self.Params[1].Type,self.Type):
        	    self.Params[1] = BoxExc(self.Params[1],self.Type)
		elif ShouldChange(self.Params[0].Type,self.Params[1].Type) != "":
        		self.Params[1] = BoxExc(self.Params[1],self.Params[0].Type)
        	else:
			raise ValueError("Type error")
        return None

class BoxFuncsCall:
    def __init__(self,Obj):
        self.Value = "~d()"
        self.Params = []
        self.Type = GetType("void")
        #self.IsSquare = False ArrFunc[]
        self.PrevId = None
        self.CallFunc = None
        #self.AsmLine = None #self.IsAsm = False   
        
        
        if Obj.Value == "d()" and Obj.Extra[0].Value == "id": 
            self.ToCall = Obj.Extra[0].Extra
            Pars = Obj.Extra[1].Extra
            if len(Pars) != 0:
                i = 0
                while i < len(Pars):
                    if Pars[i].Value != ',':
                        self.Params.append(GetUse(Pars[i]))                    
                    i += 1
        else:
            self.ToCall = Obj.Extra[1].Value
            #print(Obj.Extra[0].Value)
            if Obj.Extra[0].Value == "not":
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
    def PrintPre(self,F):
        for N in self.Params:
            N.PrintPre(F)
        self.PrevId = GetNumb()
        self.CallFunc.PrintUse(F,self.PrevId,self.Params)
        return None
    def GetName(self):
        return "Tmp{}".format(self.PrevId)
    def PrintUse(self,F):
        self.Type.PrintUse(F)
        F.write(" %Tmp{}".format(self.PrevId))
        return None
    def PrintInBlock(self,F):
        self.PrintPre(F)
    def Check(self):
        for It in self.Params:
            It.Check()

        self.CallFunc = GetFunc(self.ToCall,self.Params)
        if self.CallFunc == None:
            raise ValueError("Func not found {}".format(self.ToCall))
        self.Ret = self.CallFunc
        self.Type = self.CallFunc.Type

        for i in range(len(self.Params)):
            if self.CallFunc.Params[i].Type == GetType("..."):
                break
            if self.CallFunc.Params[i].Type.Id != self.Params[i].Type.Id and GoodPoints(self.Params[i].Type,self.CallFunc.Params[i].Type):
                self.Params[i] = BoxExc(self.Params[i],self.CallFunc.Params[i].Type)
        #print(self.Type.Id)
        return None



def GetUse(Obj):
    if Obj.Value in ["d()","dm","db"]:
        return  BoxFuncsCall(Obj)
    if Obj.Value in ["id"]:
        return  BoxParamCall(Obj)
    if Obj.Value in ["dp"]:
        return  BoxPrisvCall(Obj)
    if Obj.Value in ["d^","d[]"]: 
        return BoxPoint(Obj)
    if Obj.Value == "daif": 
        return BoxIf(Obj)
    if Obj.Value == "dawhile": 
        return BoxWhile(Obj)
    if Obj.Value == "str": 
        return ParConstStr(Obj)
    if Obj.Value in ["numf","numi","true","false"]:
        return ParConstNum(Obj)
    if Obj.Value == "ret":
        return BoxReturn(Obj)
    if Obj.Value == "newtype":
        #VERY TEMP
        AddFuncPoint(Obj.Extra[3],Obj.Extra[0].Extra)
        return None

    raise ValueError("Not implemented {}".format(Obj.Value))
    #print("Not implemented {}".format(Obj.Value))

class BoxBlock:
    def __init__(self,List):
        value ="~box"
        self.Items = []
        
        GotSome = True
        self.Items = []
        for It in List:
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
        for It in self.Items:
            if It.Value == "~:=" and It.Extra != None and  It.Extra.Value == "~Func":
                ContextStuff.append(It)
        for It in self.Items:
            It.Check()
            if It.Value == "~:=" and (It.Extra == None or It.Extra.Value != "~Func"):
                ContextStuff.append(It)
        PopC()


class BoxFunc:
    def __init__(self,Obj):
        self.Value = "~Func"
        self.Params = []
        self.Type = GetType("void")
        self.Block = None
        self.IsTemplate = False
        self.IsLocal = False
        self.AsmLine = None

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
                            else: 
                                self.Params.append(ParamChain(None,PArrs[0]))
                        elif len(PArrs) == 2:
                            self.Params.append(ParamChain(ParseType(PArrs[0]),PArrs[1]))
                        #else == 4   int x = 4
                        PArrs = []
                    else:
                        PArrs.append(Pars[j])
                    j += 1
            for c in self.Params:
                c.IsRef = False
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
            #F.write(" ") ?

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
        F.write(self.Name)
        F.write("(")
        if len(self.Params) > 0:
            self.Params[0].PrintParam(F)
            for i in range(1,len(self.Params)):
                F.write(",")
                self.Params[i].PrintParam(F)
        F.write(")\n")
        if self.Block == None:
            return None
        F.write("{\n")
        self.Block.PrintUse(F)
        if self.Type.Type == "standart":
            if self.Type.Base == "void":
                F.write("ret void\n")
        F.write("}\n")
    def PrintName(self,F):
        F.write("@")
        F.write(self.Name)
    def PrintPre(self,F):
        return None
    def PrintFPoint(self,F):
        self.Type.PrintUse(F)
        F.write("(")
        for c in range(len(self.Params)):
            if c != 0:
                F.write(",")
            self.Params[c].Type.PrintUse(F)
        F.write(")*")
    def Check(self):
        PushC()
        for It in self.Params:
            ContextStuff.append(It)
        if self.Block != None:
            self.Block.Check()
        PopC()
    def PrintType(self,F):
        self.Type.PrintUse(F)
        F.write("(")
        if len(self.Params) > 0:
            self.Params[0].Type.PrintUse(F)
            for i in range(1,len(self.Params)):
                F.write(",")
                self.Params[i].Type.PrintUse(F)
        F.write(")")
        return None



MFunc = ["add","sub","mul","div","rem"]
QFunc = ["+","-","*","/","%"]

MCmp = ["eq","ne","gt","ge","lt","le"]
QCmp = ["==","!=",">",">=","<","<="]

for j in ["float","double"]:
	for IFunc in range(len(MFunc)):
		TestAdd = BoxFunc(None)
		TestAdd.AsmLine ="%{0} = f"
		TestAdd.AsmLine += MFunc[IFunc]
		TestAdd.AsmLine += " "
		TestAdd.AsmLine += j
		TestAdd.AsmLine +=" %{1} , %{2}\n"
		TestAdd.Name = QFunc[IFunc]
		TestAdd.Type = GetType(j)
		TestAdd.Params.append(ParamChain(GetType(j),"~no"))
		TestAdd.Params.append(ParamChain(GetType(j),"~no"))
		StandartStuff.append(TestAdd)
	for ICmp in range(len(MCmp)):
		TestAdd = BoxFunc(None)
		TestAdd.AsmLine ="%{0} = fcmp "
		TestAdd.AsmLine += " u" + MCmp[ICmp] + " "
		TestAdd.AsmLine += j
		TestAdd.AsmLine +=" %{1} , %{2}\n"
		TestAdd.Name = QCmp[ICmp]
		TestAdd.Type = GetType("bool")
		TestAdd.Params.append(ParamChain(GetType(j),"~no"))
		TestAdd.Params.append(ParamChain(GetType(j),"~no"))
		StandartStuff.append(TestAdd)
		

for j in ["s","u"]:
	for i in [8,16,32,64]:
		Si =  "{}".format(i)
		for IFunc in range(len(MFunc)):
			TestAdd = BoxFunc(None)
			TestAdd.AsmLine ="%{0} = " 
			if IFunc in [3,4]:
				TestAdd.AsmLine += j
			TestAdd.AsmLine += MFunc[IFunc]
			TestAdd.AsmLine += " i" + Si +" %{1} , %{2}\n"
			TestAdd.Name = QFunc[IFunc] #"+"
			TestAdd.Type = GetType("{}{}".format(j,i))
			TestAdd.Params.append(ParamChain(GetType("{}{}".format(j,i)),"~no"))
			TestAdd.Params.append(ParamChain(GetType("{}{}".format(j,i)),"~no"))
			StandartStuff.append(TestAdd)

		for ICmp in range(len(MCmp)):
			TestAdd = BoxFunc(None)
			TestAdd.AsmLine ="%{0} = icmp "
			if ICmp in [2,3,4,5]:
				TestAdd.AsmLine += j
			TestAdd.AsmLine += MCmp[ICmp]
			TestAdd.AsmLine +=" i{}".format(i)
			TestAdd.AsmLine +=" %{1} , %{2}\n"
			TestAdd.Name = QCmp[ICmp]
			TestAdd.Type = GetType("bool")
			TestAdd.Params.append(ParamChain(GetType("{}{}".format(j,i)),"~no"))
			TestAdd.Params.append(ParamChain(GetType("{}{}".format(j,i)),"~no"))
			StandartStuff.append(TestAdd)

TestAdd = BoxFunc(None)
TestAdd.AsmLine ="%{0} = xor i1 %{1} , 1\n"
TestAdd.Name = "not"
TestAdd.Type = GetType("bool")
TestAdd.Params.append(ParamChain(GetType("bool"),"~no"))
StandartStuff.append(TestAdd)
