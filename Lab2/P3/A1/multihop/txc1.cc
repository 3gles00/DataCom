#include<cstring>
#include<omnetpp.h>

using namespace omnetpp;

class Txc1 : public cSimpleModule{
    private:
        long msgCounter;
        long numSent;
        long numReceived;
        double lossProbability;
        double avgIpg;
        double avgEndToEndDelay;
        double lastRx;
        cOutVector txVector;
        cOutVector rxVector;
        cOutVector e2eVector;
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
    
    msgCounter = 0;
    numReceived = 0;
    numSent = 0;
    lossProbability = par("lossProbability");
    txVector.setName("txVector");
    rxVector.setName("rxVector");
    e2eVector.setName("e2eVector");
    avgIpg = 0;
    avgEndToEndDelay = 0;
    lastRx = 0;
    
    WATCH(msgCounter);
    WATCH(numSent);
    WATCH(numReceived);

    // Start messaging if I an the first node
    if(getIndex() == 0){
        numSent++;
        msgCounter++;
        EV << "Scheduling first sent to a random time\n";
        char msgname[20];
        sprintf(msgname, "DATA-%ld", msgCounter);
        cMessage *newMsg = new cMessage(msgname);
        scheduleAt(par("delayTime"), newMsg);
        txVector.record(numSent);
    }
}

void Txc1::handleMessage(cMessage *msg){

    //Planning new Message
    if(getIndex() == 0){
        msgCounter++;
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
         double latency = simTime().dbl() - msg -> getCreationTime().dbl();
        e2eVector.record(latency);
        avgEndToEndDelay += latency;
        double ipg = simTime().dbl() - lastRx;
        avgIpg += ipg;
        lastRx = simTime().dbl();
        delete msg; 
    }
    else{
        if(lossProbability <= uniform(0, 1)){
            if(getIndex() != 0){
                numReceived++;
                numSent++;
                msgCounter++;
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

void Txc1::forwardMessage(cMessage *msg){
    // For this example, we always reveive in the gate with
    // a lower number out of the two we have, So we forward 
    // using our higher-numbered gate
    int n = gateSize("gate");
    int k = n-1;
    if(n > 1) k = intuniform(1, n-1); 
    EV << "Forwarding message " << msg << " on gate " << k << "\n";
    sendDelayed(msg, par("delayTime"), "gate$o", k);
}

void Txc1::finish(){
	EV << "Sent: " << numSent << endl;
	EV << "Received: " << numReceived << endl;
        if(getIndex() == 5){
        if(numReceived > 1) avgIpg = avgIpg/numReceived;
        EV << "IPG: " << avgIpg << endl;
        EV << "Latency: " << avgEndToEndDelay/numReceived << endl;
    }-

	recordScalar("#sent", numSent);
	recordScalar("#received", numReceived);
    recordScalar("#Latency ", avgEndToEndDelay/numReceived);
    recordScalar("IPG ", avgIpg);
}