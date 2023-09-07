simple Txc1{
	parameters:
		volatile double delayTime @unit(s);
		@signal[transmissionSignal](type="long");
		@statistic[transmissionSignal](title="tx_t";source="transmissionSignal";record=vector);
		@signal[receptionSignal](type="long");
		@statistic[receptionSignal](title="rx_t";source="receptionSignal";record=vector);
	gates:
		input in;
		output out;
}

network Tictoc1{
	submodules:
		tic: Txc1;
		toc: Txc1;
	connections:
		tic.out --> { delay = 100ms; } --> toc.in;
		tic.in <-- { delay = 100ms; } <-- toc.out;
}
