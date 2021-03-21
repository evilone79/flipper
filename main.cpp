#include <chrono>
#include <stdexcept>
#include <csignal>
#include "mqtt/client.h"
#include "mqtt/connect_options.h"
#include "util.h"
#include "credentials.h"

using namespace std::chrono;



std::sig_atomic_t terminate{0};
std::string build_reply_msg(const std::string& in_msg);

void signal_handler( int signal_num )
{
    terminate = 1;
}

namespace
{
    const std::string CLIENT_ID { "mike_test_client" };
    const std::string TOPIC_HOSTNAME { "hostname" };
    const std::string TOPIC_DATA_IN { "input_data" };
    const std::string TOPIC_DATA_OUT { "output_data" };

    const std::string HOST = "18.184.204.50";
    constexpr auto QOS = 0;
}

int main()
{
    std::signal(SIGINT, signal_handler);

    auto creds = credentials::load_from_file("./creds");

    // Create a client
    auto conn_opts = mqtt::connect_options_builder()
            .keep_alive_interval(seconds(10))
            .user_name(creds.username)
            .password(creds.password)
            .clean_session(true)
            .finalize();

    mqtt::client client(HOST, CLIENT_ID);

    try
    {
        util::log("Connecting client ", CLIENT_ID);
        auto rs = client.connect(conn_opts);
        util::log("Connected. MQTT ver. ", rs.get_mqtt_version());

        // Publish hostname
        auto hostname = util::get_hostname();
        util::log("Publishing hostname: ", hostname);
        auto hostname_msg = mqtt::make_message(TOPIC_HOSTNAME, hostname);
        hostname_msg->set_qos(QOS);

        client.publish(hostname_msg);

        util::log("Subscribing to topic ", TOPIC_DATA_IN);
        client.subscribe(TOPIC_DATA_IN);

        while (!terminate) {
            try {
                auto in_msg = client.consume_message();
                if (!in_msg) {
                    util::log("Failed to consume message");
                    continue;
                }

                auto num_str = in_msg->to_string();
                util::log("data received: ", num_str);

                auto reply_msg = build_reply_msg(num_str);
                util::log("replying: ", reply_msg);

                auto out_msg = mqtt::make_message(TOPIC_DATA_OUT, reply_msg);
                client.publish(out_msg);
            }
            catch (mqtt::exception& exc) {
                util::log("Handling error: ", exc.what(), " [" ,exc.get_reason_code(), "]");
            }
            catch (std::exception& exc) {
                util::log("Handling error: ", exc.what());
            }
        }

        util::log("Terminating flipper...");
        client.unsubscribe(TOPIC_DATA_IN);
        client.disconnect();
        util::log("Terminated");
    }
    catch (mqtt::exception& exc) {
        util::log("Error: ", exc.what(), " [" ,exc.get_reason_code(), "]");
        return 1;
    }

    return 0;
}

std::string build_reply_msg(const std::string& in_msg)
{
    if (in_msg.size() != 10)
        throw std::invalid_argument("Incorrect message length");

    auto number = static_cast<uint32_t>(std::strtol(in_msg.data(), nullptr, 0));
    if (number == 0)
        throw std::runtime_error("Failed to convert message to number");

    auto reply_num = util::flip_bits(number);
    char msg_buf[22] = {};
    std::snprintf(msg_buf, sizeof msg_buf, "%s_0x%x", in_msg.c_str(), reply_num);

    return msg_buf;
}