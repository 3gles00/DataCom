simple Txc1
{
	gates:
		input in;
		output out;
}
network TicToc1
{
	submodules:
		tic: Txc1;
		toc: Txc1;
	connection:
		tic.out --> { delay = 100ms; } --> toc.in;
		tic.in <-- { delay = 100ms; } <-- toc.out;
} 
