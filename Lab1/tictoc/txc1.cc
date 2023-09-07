#include<cstring>
#include<omnetpp.h>

using namespace omnetpp;

class Txc1 : public cSimpleModule{
	private:
		cMessage *event = nullptr;
		cMessage *tictocMsg = nullptr;
		long numSent;
		long numReceived;
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
	numReceived = 0;
	event = new cMessage("event");
	tictocMsg = nullptr;
	WATCH(numSent);
	WATCH(numReceived);
	transmissionSignal = registerSignal("transmissionSignal");
	receptionSignal = registerSignal("receptionSignal");
	//Determine if I am tick or Toc
	if (strcmp("tic",getName()) == 0){
		EV << "Scheduling the first send to a random time\n";
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
		emit(transmissionSignal,numSent);

	}else{
		if(strcmp("tic",getName()) == 0){
			EV << "Acknowledgement arrived";
			numReceived++;
			emit(receptionSignal,numReceived);
			delete msg;
			tictocMsg = nullptr;
			cancelEvent(event);
			tictocMsg = new cMessage("DATA");
			scheduleAt(simTime()+1.0,event);
		}else{
			EV << "Message arrived. Sending ACK";
			numReceived++;
			emit(receptionSignal,numReceived);
			delete msg;
			tictocMsg = new cMessage("ACK");
			scheduleAt(simTime()+exponential(0.1), event);
		}
	}
}

void Txc1::finish(){
	EV << "Sent: " << numSent << endl;
	EV << "Received: " << numReceived << endl;

	recordScalar("#sent", numSent);
	recordScalar("#received", numReceived);
}