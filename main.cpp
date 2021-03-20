#include <iostream>
#include <chrono>
#include <unistd.h>
#include <stdexcept>

#include "mqtt/client.h"
#include "mqtt/connect_options.h"

using namespace std::chrono;

std::string get_hostname();

bool terminate{false};
uint32_t flip_bits(uint32_t input);
std::string build_reply_msg(const std::string& in_msg);

int main()
{
    const std::string TOPIC_HOSTNAME { "hostname" };
    const std::string TOPIC_DATA_IN { "input_data" };
    const std::string TOPIC_DATA_OUT { "output_data" };

    const std::string HOST = "18.184.204.50";
    const std::string USER = "client1";
    const std::string PASS = "0G*XXzzZu_ICwqBf~BQWkwsl";
    constexpr auto QOS = 0;

    // Create a client
    auto conn_opts = mqtt::connect_options_builder()
            .keep_alive_interval(seconds(10))
            .user_name(USER)
            .password(PASS)
            .clean_session(true)
            .finalize();

    mqtt::client client(HOST, "mike_test_client");
    try
    {
        auto rs = client.connect(conn_opts);
        std::cout << "Connected. MQTT ver. " << rs.get_mqtt_version() << "\n";

        // Publish hostname
        auto hostname = get_hostname();
        std::cout << "Publishing hostname: " << hostname << "\n";
        auto hostname_msg = mqtt::make_message(TOPIC_HOSTNAME, hostname);
        hostname_msg->set_qos(QOS);

        client.publish(hostname_msg);

        // subscribe to data
        client.subscribe(TOPIC_DATA_IN);

        while (!terminate) {
            auto in_msg = client.consume_message();
            if (!in_msg) {
                std::cout << "Failed to consume message\n";
                continue;
            }

            auto num_str = in_msg->to_string();
            std::cout << "data received: " << num_str << "\n";

            auto reply_msg = build_reply_msg(num_str);
            std::cout << "replying: " << reply_msg << "\n";

            auto out_msg = mqtt::make_message(TOPIC_DATA_OUT, reply_msg);
            client.publish(out_msg);
        }

        // Disconnect
        client.disconnect();
    }
    catch (const mqtt::exception& exc) {
        std::cerr << "Error: " << exc.what() << " ["
                  << exc.get_reason_code() << "]" << std::endl;
        return 1;
    }

    return 0;
}

std::string get_hostname()
{
    static constexpr auto MAX_NAME_SIZE = 128;
    char name[MAX_NAME_SIZE] = {};
    if (gethostname(name, MAX_NAME_SIZE)) {
        throw std::runtime_error(std::string{"Failed to get hostname. "} + std::strerror(errno));
    }
    return name;
}

uint32_t flip_bits(uint32_t input)
{
    static const auto TEST_MASK = (0x1 << 31);
    static const auto ODD_MASK = 0xAAAAAAAA;
    static const auto EVEN_MASK = 0x55555555;

    return input & TEST_MASK
           ? input ^ ODD_MASK
           : input ^ EVEN_MASK;
}

std::string build_reply_msg(const std::string& in_msg)
{
    auto number = static_cast<uint32_t>(std::strtol(in_msg.data(), nullptr, 0));
    auto reply_num = flip_bits(number);
    char msg_buf[22] = {};
    std::snprintf(msg_buf, sizeof msg_buf, "%s_0x%x", in_msg.c_str(), reply_num);

    return msg_buf;
}