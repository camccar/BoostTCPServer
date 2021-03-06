/*
Example code taken from https://github.com/edenhill/librdkafka/blob/master/examples/rdkafka_example.cpp

this should be reformatted into a proper object. Probably use a library to build on top of it. Probably https://github.com/mfontanini/cppkafka

*/

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>


#include <librdkafka/rdkafkacpp.h>

namespace Kafka {

  static volatile sig_atomic_t run = 1;

  static void sigterm (int sig) {
    run = 0;
  }

  class DeliveryReport : public RdKafka::DeliveryReportCb {
  public:
    void dr_cb (RdKafka::Message &message) {
      /* If message.err() is non-zero the message delivery failed permanently
      * for the message. */
      if (message.err())
        std::cerr << "% Message delivery failed: " << message.errstr() << std::endl;
      else
        std::cerr << "% Message delivered to topic " << message.topic_name() <<
          " [" << message.partition() << "] at offset " <<
          message.offset() << std::endl;
    }
  };



  void SendKafkaMessage(std::string message, std::string brokers, std::string topic) {
      RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
      std::string errstr;

  /* Set bootstrap broker(s) as a comma-separated list of
    * host or host:port (default port 9092).
    * librdkafka will use the bootstrap brokers to acquire the full
    * set of brokers from the cluster. */
    if (conf->set("bootstrap.servers", brokers, errstr) !=
        RdKafka::Conf::CONF_OK) {
      std::cerr << errstr << std::endl;
      exit(1);
    }

  //if(conf->set(""))
    if(VariableService::Instance()->SASLenabled()){
      conf->set("security.protocol", "SASL_PLAINTEXT", errstr);
      conf->set("sasl.username", VariableService::Instance()->SASLUsername(), errstr);
      conf->set("sasl.password", VariableService::Instance()->SASLPassword(), errstr);
      conf->set("sasl.mechanism","PLAIN",errstr);
      //conf->set("sasl.kerberos.keytab","localhost:9092",errstr); // I have no idea why I need to set this but I do
      std::cout<<"Setting auth2"<<std::endl;
      /*if (conf->set("security.protocol", "SASL_PLAINTEXT", errstr) != RdKafka::Conf::CONF_OK) {
        std::cout<<"outting err auth2"<<std::endl;
        std::cerr << errstr << std::endl;
        std::cout<<"Setting protocal"<<std::endl;
        exit(1);
      }else if (conf->set("sasl.username", VariableService::Instance()->SASLUsername(), errstr) != RdKafka::Conf::CONF_OK) {
        std::cout<<"Setting username"<<std::endl;
        std::cerr << errstr << std::endl;
        exit(1);
      }else if (conf->set("sasl.password", VariableService::Instance()->SASLPassword(), errstr) != RdKafka::Conf::CONF_OK) {
        std::cout<<"Setting password"<<std::endl;
        std::cerr << errstr << std::endl;
        exit(1);
      }
      */
    }

    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);


  /* Set the delivery report callback.
    * This callback will be called once per message to inform
    * the application if delivery succeeded or failed.
    * See dr_msg_cb() above.
    * The callback is only triggered from ::poll() and ::flush().
    *
    * IMPORTANT:
    * Make sure the DeliveryReport instance outlives the Producer object,
    * either by putting it on the heap or as in this case as a stack variable
    * that will NOT go out of scope for the duration of the Producer object.
    */
    DeliveryReport DeliveryReportCallback;

    if (conf->set("dr_cb", &DeliveryReportCallback, errstr) != RdKafka::Conf::CONF_OK) {
      std::cerr << errstr << std::endl;
      exit(1);
    }

    /*
    * Create producer instance.
    */
    RdKafka::Producer *producer = RdKafka::Producer::create(conf, errstr);
    if (!producer) {
      std::cerr << "Failed to create producer: " << errstr << std::endl;
      exit(1);
    }

    delete conf;

    std::string line = message;
    producer->poll(0);// Not sure what this line does.

  retry:
      RdKafka::ErrorCode err = 
      producer->produce(
                          /* Topic name */
                          topic,
                          /* Any Partition: the builtin partitioner will be
                          * used to assign the message to a topic based
                          * on the message key, or random partition if
                          * the key is not set. */
                          RdKafka::Topic::PARTITION_UA,
                          /* Make a copy of the value */
                          RdKafka::Producer::RK_MSG_COPY /* Copy payload */,
                          /* Value */
                          const_cast<char *>(line.c_str()), line.size(),
                          /* Key */
                          NULL, 
                        0,
                          /* Timestamp (defaults to current time) */
                          0,
                          /* Message headers, if any */
                          NULL
                          /* Per-message opaque value passed to
                          * delivery report */
                          );

      if (err != RdKafka::ERR_NO_ERROR) {
        std::cerr << "% Failed to produce to topic " << topic << ": " <<
          RdKafka::err2str(err) << std::endl;

        if (err == RdKafka::ERR__QUEUE_FULL) {
          /* If the internal queue is full, wait for
          * messages to be delivered and then retry.
          * The internal queue represents both
          * messages to be sent and messages that have
          * been sent or failed, awaiting their
          * delivery report callback to be called.
          *
          * The internal queue is limited by the
          * configuration property
          * queue.buffering.max.messages */
          producer->poll(1000/*block for max 1000ms*/);
          goto retry;
        }

      } else {
        std::cerr << "% Enqueued message (" << line.size() << " bytes) " <<
          "for topic " << topic << std::endl;
      }
      producer->poll(0);

      std::cerr << "% Flushing final messages..." << std::endl;
      producer->flush(10*1000 /* wait for max 10 seconds */);

      std::cout<<"ending function"<<std::endl;
      return;
  }

}