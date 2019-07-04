
main := !(int argc, char^^ argv) -> int
{
	//c := "where "sbt + "am " + "i?"
	c := ("where "sbt +  "am " + "i?").Str()
	printf("test <%s>\n",c.Str())
	return 0
}

