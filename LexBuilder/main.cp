#import "../Libs/RegExpBuilder.cp"

main := !(int argc, char^^ argv) -> int
{
	itBuilder := LexBuilder()

	itBuilder.ApplyReg("[a-zA-Z_][a-zA-Z0-9_]*",1)
	//itBuilder.ApplyReg("[$#][a-zA-Z0-9_]*,2)
	//itBuilder.ApplyReg("\"[^\"]*\"",3)
	//itBuilder.ApplyReg("'( (\\\\ [^]) |  [^'\\\\])'",4)
	//itBuilder.ApplyReg("[0-9]+",5)
	//itBuilder.ApplyReg("[0-9]+.[0-9]+(e[+-]?[0-9]+)?",6)
	//itBuilder.ApplyReg("0x[0-9a-fA-F]+",7)
	return 0
}
