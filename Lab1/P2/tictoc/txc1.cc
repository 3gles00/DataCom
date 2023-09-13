#include <cstring>
#include <omnetpp.h>

using namespace omnetpp;

class Txc1 : public cSimpleModule{
    private:
        cMessage *event = nullptr;
        cMessage *tictocMsg = nullptr;
        long numSent;
        long numRecieved;
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();
};

Define_Module(Txc1);

Txc1::~Txc1(){
    cancelAndDelete(event);
}



void Txc1::initialize(){
    numSent = 0;
    numRecieved = 0;
    event = new cMessage("event");
    tictocMsg = nullptr;
    WATCH(numSent);
    WATCH(numRecieved);
    //Determine if I am Toc or Toc
    if(strcmp("tic", getName()) == 0){
        EV << "Scheduling first send to a random time\n";
        tictocMsg = new cMessage("DATA");
        scheduleAt(uniform(0, 1), event);
    }
}

void Txc1::handleMessage(cMessage *msg){
    if(msg == event){
        EV << "Timeout is over, sending message";
        send(tictocMsg, "out");
        tictocMsg = nullptr;
        numSent++;
    }
    else{
        if(strcmp("tic", getName()) == 0){
            EV << "Acknowledgement arrived";
            numRecieved++;
            delete msg;
            tictocMsg = nullptr;
            cancelEvent(event);
            tictocMsg = new cMessage("DATA");
            scheduleAt(simTime() + 1.0, event);
        } 
        else{
            EV << "Message Arrived. Sending ACK";
            numRecieved++;
            delete msg;
            tictocMsg = new cMessage("ACK");
            scheduleAt(simTime() + exponential(0.1), event);
        }
    }
}

void Txc1::finish(){
    
    EV << "Sent: " << numSent << endl;
    EV << "Recieved: " << numRecieved << endl;
    
    recordScalar("#sent", numSent);
    recordScalar("#received", numRecieved);

}