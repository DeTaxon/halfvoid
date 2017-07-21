class ParConstStr:
    def __init__(self):
        Value = "~str"
        Extra = None
        Size = 0
    def PrintCons(self,F):
        F.write('@CStr{} = constant [{} x i8] c"'.fromat(Numb,self.Size))
        i = 0
        while i < len(self.Extra):
            if self.Extra.Value[i] == '\\':
                i += 1
                if self.Extra[i] == 'n':
                    f.write('\\0A')
                    i+= 1
            else:
                F.write(Value[i])
                i += 1

class BoxBlock:
    def __init__(self,List):
        value ="~box"
        self.Items = []
        
        GotSome = True
        while GotSome:
            i = 0
            GotSome = False
            while i < len(List):
                if List[i].Value == ":=":
                    GotSome = True
                    break
                i+= 1
            if GotSome:
                if List[i+1].Value == '!':
                    Fu = BoxFunc(List,i)
            else:
                print("Broken := ")
                self.Tokens.pop(i)

    def PrintConst(self,F):
        for It in self.Items:
            Id.PrintConst(F)


class BoxFunc:
    def __init__(self,List,Pos):
        self.Value = "~Func?"
        self.Name = "%none%"
        self.Params = []
        self.Block = None
        
        StartDel = Pos
        if Pos > 0:
            if List[Pos - 1].Value in ["id","str"]:
                Name = List[Pos=1].Extra
                StartDel = -1
        i = 3
        while List[Pos + i].Value not in  ["{}","declare"]:
            i += 1
        if List[Pos + 1].Value != "declare":
            Block = BoxBlock[List[Pos + i].Extra)
        EndDel = Pos + i

        
    def PrintConst(self,F):
        self.Block.PrintConst(F)
