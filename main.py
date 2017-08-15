from Lex import *
from MathS import *
from Boxs import *


class CodeBox:
    def __init__(self,Line):
        self.Tokens = []

        with open(Line,"r") as f:
            PreLines = f.readlines()
        Lines = []
        for c in PreLines: #deleting comments
            pos = c.find('//')
            if pos == -1:
                PreStr = c[:-1] + ' '
            else:
                PreStr = c[:pos] + ' '
            if len(PreStr) > 0:
                Lines.append(PreStr)
        for Line in Lines:
            GenTokens(self.Tokens,Line)
        self.SearchPar('(',')','()',self.Tokens)
        self.SearchPar('[',']','[]',self.Tokens)
        self.SearchPar('{','}','{}',self.Tokens)
        self.RecSearch(self.Tokens,SearchMath)
        #self.Check()
        self.Block = BoxBlock(self.Tokens)
        self.Block.Check()
    def RecSearch(self,Arr,Fun):
        for c in Arr:
            if c.Value in ["{}","[]","()"]:
                self.RecSearch(c.Extra,Fun)
        Fun(Arr)

    def SearchPar(self,Start,End,New,Toks):
        if len(Toks) == 0:
            return
        Bag = []

        Now = -1
        Pos = 0
        
        while Pos < len(Toks):
            if Toks[Pos].Value == Start:
                if Now != -1:
                    Bag.append(Now)
                Now = Pos
            elif Toks[Pos].Value in ["{}","[]","()"]:
                self.SearchPar(Start,End,New,Toks[Pos].Extra)
            elif Toks[Pos].Value == End:
                if Now == -1:
                    return
                Block = Token(New,[])
                for j in range(Now+1,Pos):
                    Block.Extra.append(Toks.pop(Now+1))
                Toks.pop(Now)
                Toks[Now] = Block
                Pos = Now
                if len(Bag) > 0:
                    Now = Bag.pop()
                else:
                    Now = -1
            Pos += 1
    def Out(self,Name):
        with open(Name,"w") as F:
            self.Block.PrintConst(F)
            self.Block.PrintFunc(F)

    def Check(self):
        self.CheckB(self.Tokens,1)
    def CheckB(self,Arr,Size):
        for T in Arr:
            if hasattr(T,'Info'):
                print(Size*'-' + "Token {}".format(T.Info()))
            else:
                print(Size*'-' + "Token {}".format(T.Value))
            if T.Value in ["{}","()","[]"]:
                self.CheckB(T.Extra,Size+1)
            if T.Value in ForFor:
                self.CheckB(T.Extra,Size+1)

It = CodeBox("simpl.cp")
It.Out("out.ll")
