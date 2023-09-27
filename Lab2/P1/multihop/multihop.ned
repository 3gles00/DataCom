simple Txc1{
    paramaters:
        volatile double delayTime @unit(s);
    gates:
        inout gate[];
}

network Multihop{
    submodules:
        node[6]: Txc1;
    connections:
        node[0].gate++ <--> { delay = 100ms; } <--> node[1].gate++;
        node[1].gate++ <--> { delay = 100ms; } <--> node[2].gate++;
        node[2].gate++ <--> { delay = 100ms; } <--> node[3].gate++;
        node[3].gate++ <--> { delay = 100ms; } <--> node[4].gate++;
        node[4].gate++ <--> { delay = 100ms; } <--> node[5].gate++;
}