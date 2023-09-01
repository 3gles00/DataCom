#include<cstring>
#include<omnetpp.h>

using namespace omnetpp;

class Txc1 : public cSimpleModule{
    private:
        cMessage *event = nullptr;
        long numSent;
        long numReceived;
        long msgCounter;
        simsignal_t transmissionSignal;
        simsignal_t receptionSignal;
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void forwardMessage(cMessage *msg);
};

Define_Module(Txc1);

//Txc1::~Txc1(){
    //cancelAndDelete(msg);
//}

void Txc1::initialize(){
    msgCounter = 0;
    numSent = 0;
    numReceived = 0;

    WATCH(numSent);
    WATCH(numReceived);

    transmissionSignal = registerSignal("transmissionSignal");
    receptionSignal = registerSignal("receptionSignal");

    //Start messaging if I am the first node
    if(getIndex() == 0){
        // msgCounter++;
        EV << "Scheduling first send to a random time\n";
        char msgname[20];
        sprintf(msgname, "DATA-%ld", msgCounter);
        cMessage *msg = new cMessage(msgname);
        scheduleAt(par("delayTime"), msg);
    }
}

void Txc1::handleMessage(cMessage *msg){
   if(getIndex() == 5){
        //Message arrived
        EV << "Message " << msg << " arrived.\n";
        msgCounter++;
        numReceived++;
        emit(receptionSignal, numReceived);
    }
    else{
        //Message has to be forwarded
        msgCounter++;
        numReceived++;
        emit(receptionSignal, numReceived);
        
        forwardMessage(msg);
    }
}

void Txc1::forwardMessage(cMessage *msg){
    /*For this example, we always receive in the gate with
    a lower number out of the two we have. So we forward
    using our higher-numberd gate*/
    int n = gateSize("gate");
    int k = n-1;
    EV << "Forwarding message " << msg << " on gate[" << k << "]\n";
    
    msgCounter++;
    numSent++;
    emit(transmissionSignal, numSent);

    send(msg, "gate$o", k);
}
