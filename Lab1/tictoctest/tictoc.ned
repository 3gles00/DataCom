simple Txc1{
    gates:
        input in;
        output out;
}

network Tictoc1{
    submodules:
        tic: Txc1;
        toc: Tcxc1;
    connections:
        tic.out --> { delay = 100 ms; } --> toc.in;
        tic.in <-- {delay = 100 ms; } <-- toc.out;
}