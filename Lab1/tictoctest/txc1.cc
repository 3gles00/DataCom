#include <cstring>
#include <omnetpp.h>

using namespace omnetpp;

class Txc1 : publi cSimpleModule{
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
};

Define_Module(Txc1);

void Txc1::initialize(){
    
    // Determine if I am Tic or Toc
    if(strcmp("tic", getName()) == 0){
        cMessage *msg = new cMessage("tictocMsg");
        send(msg, "out");
    }
}

void Txc1::handleMessage(cMessage *msg){
    send(msg, "out");
}