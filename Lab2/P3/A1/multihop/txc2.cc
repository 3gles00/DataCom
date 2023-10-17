#include<cstring>
#include<omnetpp.h>
// #include<coutvector.h>

using namespace omnetpp;

class Txc2 : public cSimpleModule{
    private:
        long numSent;
        long numReceived;
        double lossProbability;
        cOutVector txVector;
        cOutVector rxVector;
    public:
        virtual ~Txc2();
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void forwardMessage(cMessage *msg);
        virtual void finish();
};

Define_Module(Txc2);

Txc2::~Txc2(){}

void Txc2::initialize(){
    
    numReceived = 0;
    numSent = 0;
    lossProbability = par("lossProbability");
    txVector.setName("txVector");
    rxVector.setName("rxVector");
    
    // Start messaging if I an the first node
    if(getIndex() == 0){
        numSent++;
        EV << "Scheduling first sent to a random time\n";
        char msgname[20];
        sprintf(msgname, "DATA-%ld", numSent);
        cMessage *newMsg = new cMessage(msgname);
        scheduleAt(par("delayTime"), newMsg);
        txVector.record(numSent);
    }
}

void Txc2::handleMessage(cMessage *msg){

    //Planning new Message
    if(getIndex() == 0){
        numSent++;
        EV << "Scheduling next event\n";
        char msgname[20];
        sprintf(msgname, "DATA-%ld", numSent);
        cMessage *newMsg = new cMessage(msgname);
        scheduleAt(simTime() + par("transmissionTime"), newMsg);
        txVector.record(numSent);   
    }

    // Forwarding Message
    if(getIndex() == 5){
        // Message arrived
        EV << "Message " << msg << " arrived\n";
        numReceived++;
        rxVector.record(numReceived);
        delete msg; 
    }
    else{
        if(lossProbability < uniform(0, 1)){
            if(getIndex() != 0){
                numReceived++;
                numSent++;
                rxVector.record(numReceived);
                txVector.record(numSent);
            }
            forwardMessage(msg);
        }
        else{
            EV << "Lost Transmission\n";
            delete msg;
        }
    }
}

void Txc2::forwardMessage(cMessage *msg){
    // For this example, we always reveive in the gate with
    // a lower number out of the two we have, So we forward 
    // using our higher-numbered gate
    int n = gateSize("gate");
    int k =  n - 1;
    if(getIndex() == 1){
        k = intuniform(1, n - 1);
    }
    EV << "Forwarding message " << msg << " on gate " << k << "\n";
    sendDelayed(msg, par("delayTime"), "gate$o", k);
}

void Txc2::finish(){
	EV << "Sent: " << numSent << endl;
	EV << "Received: " << numReceived << endl;

	recordScalar("#sent", numSent);
	recordScalar("#received", numReceived);
}