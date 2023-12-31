#include<cstring>
#include<omnetpp.h>
#include<vector>
#include<algorithm>

using namespace omnetpp;
using namespace std;

class Txc2 : public cSimpleModule{
    private:
        cMessage *event = nullptr;
        cMessage *multihopMsg = nullptr;
        long numSent;
        long numReceived;
        long msgCounter;
        vector<long> duplicatePackageList;
        double lossProbability;
        double avgIpg;
        double avgEndToEndDelay;
        double lastRx;
        cOutVector txVector;
        cOutVector rxVector;
        cOutVector e2eVector;
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
    duplicatePackageList.clear();
    cancelAndDelete(event);
    delete multihopMsg;
}

void Txc2::initialize(){
    msgCounter = 0;
    numReceived = 0;
    numSent = 0;
    lossProbability = par("lossProbability");
    txVector.setName("txVector");
    rxVector.setName("rxVector");
    e2eVector.setName("e2eVector");
    event = new cMessage("event");
    avgIpg = 0;
    avgEndToEndDelay = 0;
    lastRx = 0;

    WATCH(msgCounter);
    WATCH(numSent);
    WATCH(numReceived);
    
    // Start messaging if I an the first node
    if(getIndex() == 0){
        EV << "Scheduling first sent to a random time\n";
        char msgname[20];
        sprintf(msgname, "DATA-%ld", msgCounter);
        multihopMsg = new cMessage(msgname);
        scheduleAt(simTime() + par("delayTime"), event);
        duplicatePackageList.push_back(multihopMsg->getTreeId());

    }
}

void Txc2::handleMessage(cMessage *msg){
    if(msg == event){
        EV << "Timeout is over. Sending message";
        forwardMessage(multihopMsg);
        txVector.record(numSent);
        delete multihopMsg;
        multihopMsg = nullptr;

        if(getIndex() == 0){
            //Planning new Message
            EV << "Scheduling next event\n";
            msgCounter++;
            char msgname[20];
            sprintf(msgname, "DATA-%ld", msgCounter);
            multihopMsg = new cMessage(msgname);
            txVector.record(numSent);
            scheduleAt(simTime() + par("transmissionTime"), event);
            duplicatePackageList.push_back(multihopMsg->getTreeId());
        }
    }
    else{
        if(lossProbability < uniform(0, 1)){
            if(getIndex() == 7){
                // Message arrived
                EV << "Message " << msg << " arrived\n";
                numReceived++;
                rxVector.record(numReceived);
                double latency = simTime().dbl() - msg -> getCreationTime().dbl();
                e2eVector.record(latency);
                avgEndToEndDelay += latency;
                double ipg = simTime().dbl() - lastRx;
                avgIpg += ipg;
                lastRx = simTime().dbl();
                delete msg;
            }
            else{
            // Handle message
                if(find(duplicatePackageList.begin(), duplicatePackageList.end(),
                    msg->getTreeId()) != duplicatePackageList.end()){
                    EV << "This is a duplicate package. Deleting.\n";
                    numReceived++;
                    rxVector.record(numReceived);
                    delete msg;
                }
                else{
                    // Forwarding Message
                    EV << "This is the first time we receive this message.\n";
                    duplicatePackageList.push_back(msg->getTreeId());
                    forwardMessage(msg);
                    numReceived++;
                    rxVector.record(numReceived);
                    delete msg;
                }   
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
        sendDelayed(msg -> dup(), par("delayTime"), "gate$o", i);
        numSent++;
        txVector.record(numSent);
    }
}

void Txc2::finish(){

	EV << "Sent: " << numSent << endl;
	EV << "Received: " << numReceived << endl;
    if(getIndex() == 7){
        if(numReceived > 1) avgIpg = avgIpg/numReceived;
        EV << "IPG: " << avgIpg << endl;
        EV << "Latency: " << avgEndToEndDelay/numReceived << endl;
    }

	recordScalar("#Sent", numSent);
	recordScalar("#Received", numReceived);
    recordScalar("#Latency ", avgEndToEndDelay/numReceived);
    recordScalar("IPG ", avgIpg);
}

