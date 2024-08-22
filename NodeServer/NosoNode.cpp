#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include "networking.h"
#include <fstream>
#include <string>
#include <fstream>
#include <filesystem>

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
        //CheckConfigFiles(); // Something to check !
        UpdateSeedTestnetIpServers(); // Step 1: Get Seed nodes IP from DNS and save to a vector (Seed Vector) and Testnet Vector.
        CheckNosoBlocks();
        
        // Step 2: Get all Nodes IP from Seed Nodes and save to a vector (Node Vector)
    }

private:
    std::vector<std::string> SeedIpAddresses;
    std::vector<std::string> TestnetSeedIpAddresses;

    void CheckConfigFiles()
    {
        // Change config test, now is TCP PORT, but it's not needed as is checked before starting Server.
        std::cout << "Checking Config files " << std::endl;
        const std::string config_filename = "server.cfg";

        std::ifstream config_file(config_filename);

        if (config_file.good()) {
            // If the file exists do not create or modify
            std::cout << "Config file '" << config_filename << "' exists." << std::endl;
        }
        else {
            // If config file is not present, create default one and add default values
            std::ofstream new_config_file(config_filename);
            if (new_config_file.is_open()) {
                new_config_file << "Port 8080" << std::endl;
                new_config_file.close();
                std::cout << "Config file '" << config_filename << "' created with default port 8080." << std::endl;
            }
            else {
                std::cerr << "Error: Unable to create config file '" << config_filename << "'." << std::endl;
            }
        }


    }
    
    
    void CheckNosoBlocks()
    {
        std::cout << "Checking Node BLocks "  << std::endl;
        // Check local directory /NOSODATA/BLOCKS
        // Check blocks.chk and read last block
        // Calculate pending Blocks to Download
        // Connect to Seed Node and Download Blocks
        // Verify Blocks and update block.chk.

        //Step 1: Check NOSODATA/BLOCKS and block.chk file
            // Path to the NOSODATA/BLOCKS directory
        const std::filesystem::path noso_data_dir = "NOSODATA/BLOCKS";
        const std::filesystem::path blocks_chk_file = noso_data_dir / "blocks.chk";

        // Check if the directory NOSODATA/BLOCKS exists
        if (!std::filesystem::exists(noso_data_dir)) {
            // Directory doesn't exist, create it
            std::cout << "Directory NOSODATA/BLOCKS does not exist. Creating it..." << std::endl;
            if (std::filesystem::create_directories(noso_data_dir)) {
                std::cout << "Directory NOSODATA/BLOCKS created successfully." << std::endl;
            }
            else {
                std::cerr << "Error: Could not create directory NOSODATA/BLOCKS." << std::endl;
                return;
            }
        }
        else {
            std::cout << "Directory NOSODATA/BLOCKS already exists." << std::endl;
        }

        // Check if the file blocks.chk exists
        if (!std::filesystem::exists(blocks_chk_file)) {
            // File doesn't exist, create it with value 0
            std::ofstream outfile(blocks_chk_file);
            if (outfile.is_open()) {
                outfile << "0";
                outfile.close();
                std::cout << "File blocks.chk created with value 0." << std::endl;
            }
            else {
                std::cerr << "Error: Could not create file blocks.chk." << std::endl;
            }
        }
        else {
            std::cout << "File blocks.chk already exists, loading data to LastBlock." << std::endl;
          
        }

    }
    
    
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
void CheckConfigFiles()
{
    //check server.cfg file
    //check TCP port to be used by Server to listen, or use 8080 by default.




}

void show_help() {
    std::cout << "Usage: NodeServer [options]\n"
        << "Options:\n"
        << "  -h, /h or -?         Show this help message and exit\n"
        << "  -p <port>            Specify the port number to use (default: 8081)\n"
        << "  -t                   Connect to Testnet\n"
        << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        boost::asio::io_context io_context;
        // Default port 
        short port = 8080;
        short testnetPort = 4041;
        bool UseTestnet = false;
        //int LastBlock = 0;
        
        for (int i = 1; i < argc; ++i) {
            if (std::string(argv[i]) == "-t" ) {
                UseTestnet = true;
                std::cout << "**** TESTNET ***** Default Port 4040 " << std::endl;
                break;
                ////Pending Implementation.
            }
        }
        
        
        //Print Help.
        
        for (int i = 1; i < argc; ++i) {
            if (std::string(argv[i]) == "-h" || std::string(argv[i]) == "/h" || std::string(argv[i]) == "-?") {
                show_help();
                return 0;
            }
        }
       

        // Check command arguments if -p parámeter is present
        for (int i = 1; i < argc; ++i) {
            if (std::string(argv[i]) == "-p" && i + 1 < argc) {
                port = static_cast<short>(std::stoi(argv[++i]));
                break;
            }
        }

        //CheckConfigFiles();
        
        if (UseTestnet==false)
        {
            std::cout << "Starting Server on Port " << port << std::endl;
            Server server(io_context, port);
            server.Initialize(); // Start doing server initial checks and setup before going online.
            io_context.run();
        }
        else 
        {
            std::cout << "Starting Server on TESTNET Port " << testnetPort << std::endl;
            Server server(io_context, testnetPort);
            server.Initialize(); // Start doing server initial checks and setup before going online.
            io_context.run();
        }
      
        
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
