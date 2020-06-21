#include "VariableService.h"


VariableService* VariableService::m_pInstance = NULL;
using std::cout;
using std::endl;


VariableService::VariableService()
{
    _topic = "";
    _port = 0;
    portFound = false;
    topicFound = false;
    
}


VariableService* VariableService::Instance()
{
	if (!m_pInstance) // Only allow one instance of class to be generated
	{
		m_pInstance = new VariableService();
	}
	return m_pInstance;
};

void VariableService::setValues(int count, char *argv[])
{
    
    /* Check Program Arguments */
    
    for(int i = 1; i < count; ++i) {
        std::string current = argv[i];
                    if(current == "-topic" || current == "-t"){
                            if(count > i){
                                    this->_topic = argv[i+1];
                                    ++i;//Skip over next iteration
                                    topicFound = true;

                            }
                    }else if ((current == "-port" || current == "-p") && count > i) {
                        std::istringstream iss( argv[i+1] );
                        int temp_port;
                        if (iss >> temp_port){
                            _port = temp_port;
                            portFound = true;
                            ++i;

                        }else{
                                std::cout<<"Invalid Port \n";
                        }
                    } else if(current == "-h" || current == "-help"){
                        ShowUsage(argv[0]);
                        exit(1);
                    } else if((current == "-ka" || current == "-kafkaaddress" )&& count > i){
                        _kafkaaddress = argv[i+1];
                        kafkaaddressFound = true;
                    }
             }
             
             
        /* Environment variables have higher priority than command line arguments. So check those second */
        
          if(const char* env_p = std::getenv("KAFKA_OUTPUT_TOPIC")){
                this->_topic = env_p;
                topicFound = true;
          }

        if(const char* env_c = std::getenv("KAFKAADDRESS")){
                _kafkaaddress = env_c;
                kafkaaddressFound = true;
        }
        
         if(const char* env_p = std::getenv("PORT")){
                std::istringstream iss(env_p );
                        int temp_port;
                        if (iss >> temp_port){
                            _port = temp_port;
                            portFound = true;
                           

                        }else{
                                std::cout<<"Invalid Port \n";
                        }
          }
             
             
             CheckInputExitIfRequiredNotAvailable( );
};

    void VariableService::ShowUsage(char *appName) {
        cout<<appName<<" + options"<<endl;
        cout<<"\nOptions"<<endl;
        cout<<"-p or -port  <portnumber> "<<endl;
        cout<<"-t or -topic  <kafka topic> "<<endl;  
        cout<<"-ka or -kafkaaddress  <kafka address> "<<endl; 
        cout<<"\nAll Options can be over written with Enviornment variables\n"<<endl;
        cout<<"KAFKA_OUTPUT_TOPIC"<<endl;
        cout<<"KAFKAADDRESS"<<endl;
        cout<<"PORT"<<endl;
    }

void VariableService::CheckInputExitIfRequiredNotAvailable(){
    if(!this->portFound){
            cout<<"Port Number Required"<<endl;
            cout<<"Type -h or -help to see options"<<endl;
            exit(1);
    }
    if(!this->topicFound || !this->kafkaaddressFound){
            cout<<"Kafka Address and topic is required to forward to kafka. No forwarding will take place.\n";
            cout<<"Type -h or -help to see options"<<endl;
    }
}

int VariableService::Port(){
        return _port;
}

 std::string VariableService::Kafkaaddress(){
     return _kafkaaddress;
 }
std::string VariableService::Topic(){
        return _topic;
}

bool VariableService::kafkaenabled(){
    return (kafkaaddressFound && topicFound);
}
    

