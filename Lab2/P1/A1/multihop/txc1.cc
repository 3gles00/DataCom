#include<cstring>
#include<omnetpp.h>

using namespace omnetpp;

class Txc1 : public cSimpleModule{
    private:
        // char firstMessageFlag;
        // long msgCounter;
        long numSent;
        long numReceived;
        simsignal_t transmissionSignal;
        simsignal_t receptionSignal;
    public:
        virtual ~Txc1();
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void forwardMessage(cMessage *msg);
        virtual void finish();
};

Define_Module(Txc1);

Txc1::~Txc1(){

}

void Txc1::initialize(){
    // msgCounter = 0;
    //firstMessageFlag = 0;
    numSent = 0;
    numReceived = 0;
    // WATCH(msgCounter);
    WATCH(numSent);
    WATCH(numReceived);
    transmissionSignal = registerSignal("transmissionSignal");
    receptionSignal = registerSignal("receptionSignal");

    // Start messaging if I an the first node
    if(getIndex() == 0){
        numSent++;
        // msgCounter++;
        EV << "Scheduling first sent to a random time\n";
        char msgname[20];
        sprintf(msgname, "DATA-%ld", numSent);
        cMessage *msg = new cMessage(msgname);
        scheduleAt(par("delayTime"), msg);
        emit(transmissionSignal, numSent);
    }
}

void Txc1::handleMessage(cMessage *msg){

    //Planning new Message
    if(getIndex() == 0){
        // numSent++;
        EV << "Scheduling 1 second after last event\n";
        char msgname[20];
        sprintf(msgname, "DATA-%ld", numSent);
        cMessage *newMsg = new cMessage(msgname);
        scheduleAt(simTime() + 1, newMsg);      
    }

    // Forwarding Message
    if(getIndex() == 5){
        // Message arrived
        EV << "Message " << msg << " arrived\n";
        numReceived++;
        // msgCounter++;
        emit(receptionSignal, numReceived);
        cancelEvent(msg);
        delete msg; 
    }
    else{
        // Message has to be forwarded   
        if(getIndex() != 0){
            numReceived++;
            emit(receptionSignal, numReceived);
        }
        numSent++;
        emit(transmissionSignal, numSent);

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

void Txc1::finish(){
    EV << "Sent: " << numSent << endl;
    EV << "Received: " << numReceived << endl;
    //EV << "msgCounter: " << msgCounter << endl;

    recordScalar("#sent", numSent);
    recordScalar("#received", numReceived);
    // recordScalar("#counter", msgCounter);
}