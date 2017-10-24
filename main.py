import sys
from Lex import *
from MathS import *
from Boxs import *


class CodeBox:
    def __init__(self,Line):
        self.Tokens = []

	j = 0
	for g in range(1,len(Line)):
		PreLines = []
        	with open(Line[g],"r") as f:
            		PreLines += f.readlines()
        	Lines = []
        	for c in PreLines: 
        	    pos = c.find('//')
        	    if pos == -1:
        	        PreStr = c[:-1] + ' '
        	    else:
        	        PreStr = c[:pos] + ' '
        	    Lines.append(PreStr)
        	for i in range(len(Lines)):
        	    GenTokens(self.Tokens,Lines[i])
		    while j < len(self.Tokens):
			self.Tokens[j].Line = i + 1
			self.Tokens[j].InFile = Line[g]
			j += 1
        self.SearchPar('(',')','()',self.Tokens)
        self.SearchPar('[',']','[]',self.Tokens)
        self.SearchPar('{','}','{}',self.Tokens)
        self.RecSearch(self.Tokens,SearchMath)
        self.Block = BoxBlock(self.Tokens)
	self.Block.MakeGlobal()
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
		Block.Line = Toks[Now + 1].Line
		Block.InFile = Toks[Now + 1].InFile
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
	    F.write("%OpaqueType = type opaque\n")
            self.Block.PrintConst(F)
	    self.Block.PrintGlobal(F)
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

It = CodeBox(sys.argv)
It.Out("out.ll")
