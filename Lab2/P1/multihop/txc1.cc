#include<cstring>
#include<omnetpp.h>

using namespace omnetpp;

class Txc1 : public cSimpleModule{
    private:
        long msgCounter;
        // cMessage *event;
    public:
        virtual ~Txc1();
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void forwardMessage(cMessage *msg);
};

Define_Module(Txc1);

Txc1::~Txc1(){
    // cancelAndDelete(msg);
    // delete msgCounter;
}

void Txc1::initialize(){
  msgCounter = 0;
  // Start messaging if I an the first node
  if(getIndex() == 0){
    msgCounter++;
    EV << "Scheduling first sent to a random time\n";
    char msgname[20];
    sprintf(msgname, "DATA-%ld", msgCounter);
    cMessage *msg = new cMessage(msgname);
    scheduleAt(par("delayTime"), msg);
  }
}

void Txc1::handleMessage(cMessage *msg){
    if(getIndex() == 5){
        // Message arrived
        EV << "Message " << msg << " arrived\n";
        cancelEvent(msg);
        delete msg;
        // Planning new message
        msgCounter++;
        char msgname[20];
        sprintf(msgname, "DATA-%ld", msgCounter);
        cMessage *msg = new cMessage(msgname);
        scheduleAt(par("delayTime") + simTime() + 1.0, msg);
    }
    else{
        // Message has to be forwarded
        forwardMessage(msg);
    }
}

void Txc1::forwardMessage(cMessage *msg){
    // For this example, we always reveive in the gate with
    // a lower number out of the two we have, So we forward 
    // using our higher-numbered gate
    int n = gateSize("gate");
    int k = n - 1;
    EV << "Forwarding message " << msg << " on gate " << k << "\n";
    send(msg, "gate$o", k);
}
