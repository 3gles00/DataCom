#include <cstring>
#include <omnetpp.h>

using namespace omnetpp;

class Txc1 : public cSimpleModule{
    private:
        long numSent;
        long numRecieved;
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();
};

Define_Module(Txc1);

void Txc1::initialize(){
    numSent = 0;
    numRecieved = 0;
    WATCH(numSent);
    WATCH(numRecieved);
    //Determine if I am Toc or Toc
    if(strcmp("tic", getName()) == 0){
        cMessage *msg = new cMessage("tictocMsg");
        send(msg, "out");
        numSent++;
    }
}

void Txc1::handleMessage(cMessage *msg){
    numRecieved++;
    send(msg, "out");
    numSent++;
}

void Txc1::finish(){
    
    EV << "Sent: " << numSent << endl;
    EV << "Recieved: " << numRecieved << endl;
    
    recordScalar("#sent", numSent);
    recordScalar("#received", numRecieved);

}