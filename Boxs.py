from Lex import *
from MathS import *
from allnums import *
from Type import *

GlobalFuncs = []

class ParamChain:
    def __init__(self,Ty,Na):
        self.Type = Ty
        self.Name = None
        
        if Na != None:
            if Na.Value == "id":
                self.Name = Na.Extra
            else:
                print("Error type")

    def PrintParam(self,F):
        if self.Type != None:
            self.Type.PrintUse(F)
        F.write(" ")
        if self.Name != None:
            F.write("%")
            F.write(self.Name)


class ParConstStr:
    def __init__(self,Obj):
        self.Value = "~str"
        self.Extra = Obj.Extra
        self.Type = "str"
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
    def PrintUse(self,F):
        F.write("i8* %LStr{}".format(self.Id))
class ParConstNum:
    def __init__(self,Obj):
        self.Value = "~num"
        self.Extra = Obj.Extra
        
        if Obj.Value == "numi":
            self.Type = GetType("int")
        if Obj.Value == "numf":
            self.Type = GetType("float")
        def PrintConst(self,F):
            return None
    def PrintConst(self,F):
        return None
    def PrintFunc(self,F):
        return None
    def PrintPre(self,F):
        return None
    def PrintUse(self,F):
        self.Type.PrintUse(F)
        F.write(" {}".format(self.Extra))
        return None

class BoxFuncsCall:
    def __init__(self,Obj):
        self.Value = "~d()"
        self.Params = []
        self.Ret = GetType("void")
        self.IsSquare = False
        
        if Obj.Extra[0].Value == "id": 
            self.ToCall = Obj.Extra[0].Extra
        else:
            print("WIP")
        Pars = Obj.Extra[1].Extra
        if len(Pars) != 0:
            #Pars.append(',')
            i = 0
            Par = {None,None}
            while i < len(Pars):
                if Pars[i].Value != ',':
                    self.Params.append(GetUse(Pars[i]))                    
                i += 1
    def PrintConst(self,F):
        for P in self.Params:
            P.PrintConst(F)
    def PrintFunc(self,F):
        for P in self.Params:
            P.PrintFunc(F)
    def PrintPre(self,F):
        for N in self.Params:
            N.PrintPre(F)
        return None
    def PrintUse(self,F,ForgetPre = False):
        CallFunc = None
        for It in GlobalFuncs:
            if It.Name == self.ToCall:
                CallFunc = It
                break
        if CallFunc == None:
            print("Nope")
            return None
        if ForgetPre == False:
            self.PrintPre(F)
        F.write("call ")
        CallFunc.PrintType(F)
        F.write(" ")
        CallFunc.PrintName(F)
        F.write("(")
        if len(self.Params) > 0:
            self.Params[0].PrintUse(F)
            for i in range(1,len(self.Params)):
                F.write(",")
                self.Params[i].PrintUse(F)
        F.write(")\n")

        return None



def GetUse(Obj):
    if Obj.Value == "d()":
        return  BoxFuncsCall(Obj)
    if Obj.Value == "str":
        return ParConstStr(Obj)
    if Obj.Value in ["numf","numi"]:
        return ParConstNum(Obj)
    print("Not implemented {}".format(Obj.Value))
    return None

class BoxBlock:
    def __init__(self,List):
        value ="~box"
        self.Items = []
        
        GotSome = True
        #while GotSome:
        #    i = 0
        #    GotSome = False
        #    while i < len(List):
        #        if List[i].Value == ":=":
        #            GotSome = True
        #            break
        #        i+= 1
        #    if GotSome:
        #        if List[i+1].Value == '!':
        #            Fu = BoxFunc(List,i)
        #        else:
        #            print("Broken := ")
        #            List.pop(i)
        #for i in range(len(List)):
        #    if List[i].Value in Box:
        #        Ni = GetUse(List[i])
        #        if Ni != None:
        #            List[i] = Ni
        #self.Items = List
        for It in List:
            print(It.Value)

    def PrintConst(self,F):
        for It in self.Items:
            It.PrintConst(F)
    def PrintFunc(self,F):
        for It in self.Items:
            It.PrintFunc(F)
    def PrintUse(self,F):
        for It in self.Items:
            It.PrintUse(F)


class BoxFunc:
    def __init__(self,List,Pos):
        self.Value = "~Func"
        self.Name = "%none%"
        self.Params = []
        self.RetType = GetType("void")
        self.Block = None
        self.IsTemplate = False
        self.IsLocal = False
        
        StartDel = Pos
        if Pos > 0:
            if List[Pos - 1].Value in ["id","str"]:
                self.Name = List[Pos-1].Extra
                StartDel = -1
        if List[Pos + 2].Value == "()":
            Pars = List[Pos + 2].Extra
            if len(Pars) > 0:
                Pars.append(Token(',',','))
                PArrs = []
                j = 0
                while j < len(Pars):
                    if Pars[j].Value == ',':
                        if len(PArrs) == 1:
                            if PArrs[0].Value == "...":
                                self.Params.append(ParamChain(GetType("..."),None))
                            else: 
                                self.Params.append(ParamChain(None,PArrs[0]))
                        elif len(PArrs) == 2:
                            self.Params.append(ParamChain(ParseType(PArrs[0]),PArrs[1]))
                        #else == 4   int x = 4
                        PArrs = []
                    else:
                        PArrs.append(Pars[j])
                    j += 1
        i = 3
        while List[Pos + i].Value not in  ["{}","declare"]:
            i += 1
        if List[Pos + i].Value != "declare":
            self.Block = BoxBlock(List[Pos + i].Extra)
        EndDel = Pos + i
        for i in range(Pos,EndDel):
            List.pop(Pos + 1)
        List[Pos] = self
        List.pop(Pos-1)        
    def PrintConst(self,F):
        if self.Block != None:
            self.Block.PrintConst(F)
    def PrintFunc(self,F):
        if self.IsTemplate:
            return None
        if self.Block != None:
            self.Block.PrintFunc(F)
        if self.Block == None:
            F.write("declare ")
        else:
            F.write("define ")
        self.RetType.PrintUse(F)
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
        if self.RetType.Type == "standart":
            if self.RetType.Base == "void":
                F.write("ret void\n")
        F.write("}\n")
    def PrintName(self,F):
        F.write("@")
        F.write(self.Name)
    def PrintPre(self,F):
        return None
    def PrintUse(self,F):
        return None
    def PrintType(self,F):
        self.RetType.PrintUse(F)
        F.write("(")
        if len(self.Params) > 0:
            self.Params[0].Type.PrintUse(F)
            for i in range(1,len(self.Params)):
                F.write(",")
                self.Params[i].Type.PrintUse(F)
        F.write(")")
        return None

def MakeGlobal(Bl):
    for Item in Bl.Items:
        if Item.Value == "~Func":
            GlobalFuncs.append(Item)

