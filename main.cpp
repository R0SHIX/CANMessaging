#include <iostream>
#include <PCANBasic.h>
#include <iomanip>
#include <cstdint>
#include <thread>
#include <chrono>
#include <cstring>


#define PCAN_CHANNEL PCAN_USBBUS1
#define PCAN_BAUDRATE PCAN_BAUD_250K

//Error Messages
#define INIT_ERROR_MSG "Error initializing PCAN-USB: "
#define READ_ERROR_MSG "Error reading message: "
#define FILTER_ERROR_MSG "Error setting message filters: "

TPCANStatus SendMessage(TPCANHandle& channel, uint32_t id, uint8_t* data, uint8_t length);
//Order I am proccessing:
//Initialize the CAN controller
// Filters
// Read/write messages
int main() {
	static const uint8_t LEN = 8;
	static const uint16_t ID = 0x608;
	static const bool KEEP_RUNNING = true;

	//std::cout << "Hello Test " << std::endl;
	TPCANStatus result;
	TPCANMsg message;
	TPCANTimestamp timestamp;
	TPCANHandle channel = PCAN_USBBUS1;
	std::cout << "Currently attempting to initalize " << std::endl;
	std::cout << "Current message values: " << " ID: " << message.ID << " Data: " << message.DATA << " LEN: " << message.LEN << std::endl;

	//Initialize
	result = CAN_Initialize(PCAN_CHANNEL, PCAN_BAUDRATE);
	std::cout << result << std::endl;
	if(result != PCAN_ERROR_OK){
		std::cerr<< INIT_ERROR_MSG << result << std::endl;
		return 1;

	}
	std::cout << "Currently attempting to filtering " << std::endl;
	
	while(KEEP_RUNNING){

		//Prepare the message
		uint8_t sendData[LEN] = {0x42,0x15,0x45,0x00,0x00,0x00,0x00,0x00};
		result = SendMessage(channel,0x608,sendData, LEN);

		if(result != PCAN_ERROR_OK){
			std::cerr << "Error sending message: " << result << std::endl;

		}else{
			std::cout << "Message sent successfully" << std::endl;
		}
		
		DWORD fromID =0;
		DWORD toID = 0x1FFFFFFF;

	//Setup filter
		result = CAN_FilterMessages(PCAN_CHANNEL,fromID,toID,PCAN_MODE_STANDARD);
		if(result != PCAN_ERROR_OK){
			std::cerr << FILTER_ERROR_MSG << result << std::endl;
			CAN_Uninitialize(PCAN_CHANNEL);
			return 1;

		}	
	
		std::cout << "Currently attempting to read messages " << std::endl;


		auto start = std::chrono::steady_clock::now();
	//Start reading message
		while(std::chrono::steady_clock::now() -start < std::chrono::seconds(1)) {

			result = CAN_Read(PCAN_CHANNEL, &message, &timestamp);
	//	std::cout << "Obtain result message" << std::endl;
			std::cout << result << std::endl;
			std::cout << "New message values: " << " ID: " << message.ID << " Data: " << message.DATA << " LEN: " << message.LEN << std::endl;
			if(result == PCAN_ERROR_OK) {

				std::cout <<  "Recieved message. ID: " << std::hex << message.ID << std::endl;
				std::cout << "Data: ";
				for( int i = 0; i <message.LEN; i++){
					std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(message.DATA[i]) << " ";

				}
				std::cout << std::endl;	
			}
			else if(result !=  PCAN_ERROR_QRCVEMPTY){
				std::cout << "Error reading message: " << result << std::endl;
			}	
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		
			if(result == PCAN_ERROR_QRCVEMPTY){
				std::cout << "Error reading message. Code: " <<result << std::endl;
		       		char buffer[256];
				CAN_GetErrorText(result, 0, buffer);
				std::cout << "Error description: " << buffer << std::endl;
			
			}
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}
	CAN_Uninitialize(PCAN_CHANNEL);

	return 0;
}

TPCANStatus SendMessage(TPCANHandle& channel, uint32_t id, uint8_t* data, uint8_t length){
	TPCANMsg msg;
	msg.ID = id;
	msg.MSGTYPE = PCAN_MESSAGE_STANDARD;
	msg.LEN = length;
	memcpy(msg.DATA,data, length);
	return CAN_Write(channel, &msg);

}
