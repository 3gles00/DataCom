#include<cstring>
#include<omnetpp.h>
#include<vector>
#include<algorithm>

using namespace omnetpp;

class Txc2 : public cSimpleModule{
    private:
        cMessage *event = nullptr;
        cMessage *multihopMsg = nullptr;
        long msgCounter;
        long numSent;
        long numReceived;
        std::vector<long> duplicatePacketList;
        double lossProbability;
        cOutVector txVector;
        cOutVector rxVector;
        // simsignal_t transmissionSignal;
        // simsignal_t receptionSignal;
    public:
        virtual ~Txc2();
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void forwardMessage(cMessage *msg);
        virtual void finish();
};

Define_Module(Txc2);

Txc2::~Txc2(){
    cancelAndDelete(event);
    delete multihopMsg;
}

void Txc2::initialize(){
    
    msgCounter = 0;
    numReceived = 0;
    numSent = 0;
    lossProbability = par("lossProbability");
    event = new cMessage("event");
    txVector.setName("txVector");
    rxVector.setName("rxVector");
    
    // Start messaging if I an the first node
    if(getIndex() == 0){
        numSent++;
        msgCounter++;
        EV << "Scheduling first sent to a random time\n";
        char msgname[20];
        sprintf(msgname, "DATA-%ld", msgCounter);
        multihopMsg = new cMessage(msgname);
        scheduleAt(par("delayTime"), event);
        txVector.record(numSent);
    }
}

void Txc2::handleMessage(cMessage *msg){

         //Planning new Message
        if(getIndex() == 0){
            msgCounter++;
            numSent++;
            EV << "Scheduling next event\n";
            char msgname[20];
            sprintf(msgname, "DATA-%ld", numSent);
            cMessage *newMsg = new cMessage(msgname);
            duplicatePacketList.push_back(newMsg->getTreeId());
            scheduleAt(simTime() + par("delayTime"), newMsg);
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
            if(getIndex)
            if(std::find(duplicatePacketList.begin(), duplicatePacketList.end(), msg->getTreeId())
                != duplicatePacketList.end()){
                EV << "This is a duplicate packet. Deleting.\n";
                delete msg;
            }
            else{
                EV << "This is the first time we reeceive this message.\n";
                duplicatePacketList.push_back(msg->getTreeId());
                numReceived++;
                msgCounter++;
                forwardMessage(msg);
                // rxVector.record(numReceived);
                // txVector.record(numSent);
            }    
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
    for(int i = 0; i < n; i++){
        send(msg -> dup(), "gate$o", i);
        numSent++;
    }
}

void Txc2::finish(){
	EV << "Sent: " << numSent << endl;
	EV << "Received: " << numReceived << endl;
	EV << "msgCounter: " << msgCounter << endl;

	recordScalar("#sent", numSent);
	recordScalar("#received", numReceived);
	// recordScalar("#counter", msgCounter);
}