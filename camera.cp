

Camera := class
{
	Dot := Cent
	Pos,At,VRight := Vec4
	Up,Right := float
	this := !() -> void
	{
		Up = 0.0
		Right = 0.0
		Pos.Set(0.0,0.0,0.0,0.0)
	}	
	Update := !() -> void
	{
		Dot.SetAng(Up,Vec3(1.0,0.0,0.0))
		Dot.Ang *= Quant(Right,Vec3(0.0,1.0,0.0))
		NewPos := Pos
		Dot.Ang.Move(NewPos)
		Dot.Pos.Set(-NewPos.x,-NewPos.y,-NewPos.z)
	}
	Set := !(Cent t) -> void
	{
		t =  Dot
	}
	Mouse := !(float x,float y) -> void // TODO: int
	{
		Up += x
		Right += y
	}
	Local := !(Vec3 To) -> void
	{
		Pre := Vec4
		for 3 Pre.vec[it] = To.vec[it]
		Pre.w = 0.0
		Dot.Ang.UnMove(Pre)
		Pos += Pre		
	} 
}
