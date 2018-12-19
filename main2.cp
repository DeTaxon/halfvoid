
printNode := !(TreeNode^ nd, int d) -> int
{
	for nd.Childs{
		for d printf("->")
		if not it.first
		{
			asStr := it.second->{string}
			printf("value {%s}\n",asStr)
		}else{
			asNd :=it.second->{TreeNode^}
			printf("subnode {%s}\n",asNd.NodeName)
			for v,k : asNd.Attrs
			{
				printf("-attr %s as %s\n",k,v)
			}
			//if asNd.NodeName == "types" {
			//	printNode(asNd,d+1)
			//}
		}
	}
}

main := !(int argc, char^^ argv) -> int
{
	itFile := MappedFile("vk.xml")
	res := ParseXML(itFile.point)

	printf("nodeName %s\n",res.NodeName)
	printNode(res,0)
	return 0
}

