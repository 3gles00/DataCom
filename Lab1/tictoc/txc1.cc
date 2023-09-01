#include<cstring>
#include<omnetpp.h>

using namespace omnetpp;

class Txc1 : public cSimpleModule{
	private:
		long numSent;
		long numReceived;
	protected:
		virtual void initialize();
		virtual void handleMessage(cMessage *msg);
		virtual void finish();
};

Define_Module(Txc1);

void Txc1::initialize(){
	numSent = 0;
	numReceived = 0;
	WATCH(numSent);
	WATCH(numReceived);
	//Determine if I am tick or Toc
	if (strcmp("tic",getName()) == 0){
		cMessage *msg = new cMessage("tictocMsg");
		send(msg, "out");
		numSent++;
	}
}

void Txc1::handleMessage(cMessage *msg){
	numReceived++;
	send(msg, "out");
	numSent++;
}

void Txc1::finish(){
	EV << "Sent: " << numSent << endl;
	EV << "Received: " << numReceived << endl;

	recordScalar("#sent", numSent);
	recordScalar("#received", numReceived);
}