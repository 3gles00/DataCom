simple Txc1{
	gates:
		input in;
		output out;
}

network Tictoc1{
	submodules:
		tic: Txc1;
		toc: Txc1;
	connections:
		tic.out --> { delay = 1000ms; } --> toc.in;
		tic.in <-- { delay = 10ms; } <-- toc.out;
}
