#include <cstring>
#include <omnetpp.h>

using namespace omnetpp;

class Txc1 : public cSimpleModule{
    private:
        cMessage *event = nullptr;
        cMessage *tictocMsg = nullptr;
        long numSent;
        long numRecieved;
        double lossProbability;
        simsignal_t transmissionSignal;
        simsignal_t receptionSignal;
    public:
        virtual ~Txc1();
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();
};

Define_Module(Txc1);

Txc1::~Txc1(){
    cancelAndDelete(event);
    delete tictocMsg;
}

void Txc1::initialize(){
    numSent = 0;
    numRecieved = 0;
    event = new cMessage("event");
    tictocMsg = nullptr;
    lossProbability = par("lossProbability");
    WATCH(numSent);
    WATCH(numRecieved);
    transmissionSignal = registerSignal("transmissionSignal");
    receptionSignal = registerSignal("receptionSignal");
    //Determine if I am Tic or Toc
    if(strcmp("tic", getName()) == 0){
        EV << "Scheduling first send to a random time\n";
        tictocMsg = new cMessage("DATA");
        scheduleAt(par("delayTime"), event);
    }
}

void Txc1::handleMessage(cMessage *msg){
    if(msg == event){
        EV << "Timeout is over, sending message";
        send(tictocMsg, "out");
        tictocMsg = nullptr;
        numSent++;
        emit(transmissionSignal, numSent);
        if(strcmp("tic", getName()) == 0){
            tictocMsg = new cMessage("DATA");
            scheduleAt(simTime()+1.0, event);
        }
    }
    else{
        if(strcmp("tic", getName()) == 0){
            EV << "Acknowledgement arrived";
            numRecieved++;
            emit(receptionSignal, numRecieved);
            delete msg;
            tictocMsg = nullptr;
            cancelEvent(event);
            tictocMsg = new cMessage("DATA");
            scheduleAt(simTime() + 1.0, event);
        }
        else{
            if(uniform(0, 1) < lossProbability){
                EV << "Message is lost";
                delete msg;
            }
            else{
                EV << "Message Arrived. Sending ACK";
                numRecieved++;
                emit(receptionSignal, numRecieved);
                delete msg;

            }
            // tictocMsg = new cMessage("ACK");
            // scheduleAt(simTime() + exponential(0.1), event);
            // scheduleAt(simTime() + par("delayTime"), event);        
        }
    }
}

void Txc1::finish(){
    
    EV << "Sent: " << numSent << endl;
    EV << "Recieved: " << numRecieved << endl;
    
    recordScalar("#sent", numSent);
    recordScalar("#received", numRecieved);

}