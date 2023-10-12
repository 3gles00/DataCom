simple Txc1{
    parameters:
        volatile double delayTime @unit(s);
        volatile double transmissionTime @unit(s);
        volatile double lossProbability;
        @signal[transmissionSignal](type="long");
		@statistic[transmissionSignal](title="tx_t";source="transmissionSignal";record=vector);
		@signal[receptionSignal](type="long");
		@statistic[receptionSignal](title="rx_t";source="receptionSignal";record=vector);
    gates:
        inout gate[];
}

network Multihop{
    submodules:
        node[8]: Txc1;
    connections:
        node[0].gate++ <--> { delay = 10ms; } <--> node[1].gate++;
        node[0].gate++ <--> { delay = 10ms; } <--> node[2].gate++;
        node[2].gate++ <--> { delay = 10ms; } <--> node[4].gate++;
        node[1].gate++ <--> { delay = 10ms; } <--> node[3].gate++;
        node[1].gate++ <--> { delay = 10ms; } <--> node[4].gate++;
        node[3].gate++ <--> { delay = 10ms; } <--> node[5].gate++;
        node[3].gate++ <--> { delay = 10ms; } <--> node[6].gate++;
        node[4].gate++ <--> { delay = 10ms; } <--> node[6].gate++;
        node[5].gate++ <--> { delay = 10ms; } <--> node[7].gate++;
        node[6].gate++ <--> { delay = 10ms; } <--> node[7].gate++;
}