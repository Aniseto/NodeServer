#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include "networking.h"

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, int session_id)
        : socket_(std::move(socket)), session_id_(session_id) {}

    void start() {
        std::cout << "Session " << session_id_ << " started." << std::endl;
        do_read();
    }

private:
    void do_read() {
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    do_write(length);
                }
            });
    }

    void do_write(std::size_t length) {
        auto self(shared_from_this());
        boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
            [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    do_read();
                }
            });
    }

    tcp::socket socket_;
    int session_id_;  // Unique identifier for each session
    enum { max_length = 1024 };
    char data_[max_length];
};

class Server {
public:
    Server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), session_counter_(10000) {
        do_accept();
    }

    void Initialize() {
        std::cout << "Initializing Node" << std::endl;
        UpdateSeedTestnetIpServers();

        // Step 1: Get Seed nodes IP from DNS and save to a vector (Seed Vector)
        // Step 2: Get all Nodes IP from Seed Nodes and save to a vector (Node Vector)
    }

private:
    std::vector<std::string> SeedIpAddresses;
    std::vector<std::string> TestnetSeedIpAddresses;

    void UpdateSeedTestnetIpServers() {
        std::string SeedServersDNS = "seed.nosocoin.com";
        std::string TestnetServersDNS = "testnet.nosocoin.com";

        tcp::resolver resolver(acceptor_.get_executor());
        // Usar get_io_context() en lugar de get_executor()

        try {
            // Check Seed Nodes and update Vector.
            tcp::resolver::results_type Seedresults = resolver.resolve(SeedServersDNS, "");
            for (const auto& entry : Seedresults) {
                std::string ip_address = entry.endpoint().address().to_string();
                SeedIpAddresses.push_back(ip_address);
                std::cout << "Added Seed Servers Resolved IP: " << ip_address << std::endl;
            }

            // Check Testnet Nodes and update Vector
            tcp::resolver::results_type Testnetresults = resolver.resolve(TestnetServersDNS, "");
            for (const auto& entry : Testnetresults) {
                std::string ip_address = entry.endpoint().address().to_string();
                TestnetSeedIpAddresses.push_back(ip_address);
                std::cout << "Added Testnet Servers Resolved IP: " << ip_address << std::endl;
            }
        }
        catch (std::exception& e) {
            std::cerr << "DNS Resolution failed: " << e.what() << std::endl;
        }
    
    }

    void do_accept() {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    // Assign a unique session ID to each new session
                    std::make_shared<Session>(std::move(socket), session_counter_++)->start();
                }
                do_accept();
            });
    }

    tcp::acceptor acceptor_;
    int session_counter_;  // Counter to generate unique session IDs
};

int main() {
    try {
        boost::asio::io_context io_context;

        Server server(io_context, 12345);
        server.Initialize();
        io_context.run();

    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
