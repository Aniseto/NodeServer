#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include "networking.h"
#include <fstream>
#include <string>
#include <fstream>
#include <filesystem>
#include <queue>  




using boost::asio::ip::tcp;

class SeedConnection : public std::enable_shared_from_this<SeedConnection> {
public:

    SeedConnection(boost::asio::io_context& io_context, const std::string& server_ip)
        : socket_(io_context), server_ip_(server_ip), timer_(io_context), is_writing_(false), is_processing_(false) {}


    void start() {
        auto self(shared_from_this());
        tcp::resolver resolver(socket_.get_executor());
        auto endpoints = resolver.resolve(server_ip_, "8080");  // Assuming port 8080 for simplicity
        boost::asio::async_connect(socket_, endpoints,
            [this, self](boost::system::error_code ec, tcp::endpoint) {
                if (!ec) {
                    std::cout << "Connected to " << server_ip_ << std::endl;
                    SaveLogToFile("ConnectedIp.txt", server_ip_);
                    do_write(Get_Ping_Message());
                    start_ping_timer(); // Start the timer to send $PING every 5 seconds
                    do_read();  // Start reading from the server
                }
                else {
                    std::cout << "Error connecting to " << server_ip_ << ": " << ec.message() << std::endl;
                }
            });
    }
    void SaveLogToFile(const std::string& filename, const std::string& ip) {
        std::ifstream infile(filename);
        std::string line;
        bool found = false;

        while (std::getline(infile, line)) {
            if (line == ip) {
                found = true;
                break;
            }
        }

        infile.close();

        if (!found) {
            std::ofstream outfile(filename, std::ios_base::app); // Open in append mode
            if (outfile.is_open()) {
                outfile << ip << std::endl;
                outfile.close();
            }
            else {
                std::cerr << "Error opening file " << filename << " for writing." << std::endl;
            }
        }
    }

private:
    
    
    void start_ping_timer() {
        auto self(shared_from_this());

        // Set the timer to expire after 5 seconds
        timer_.expires_after(std::chrono::seconds(5));

        timer_.async_wait([this, self](boost::system::error_code ec) {
            if (!ec) {
                // Send a $PING message
                do_write(Get_Ping_Message());

                // Start reading after sending the message to process responses
                do_read();

                // Schedule the next ping
                start_ping_timer();  // Recursive call to send the next ping
            }
            else {
                std::cout << "Timer error: " << ec.message() << std::endl;
            }
            });
    }



    void process_incoming_message() {
        if (incoming_message_queue_.empty() || is_processing_) {
            return;
        }

        is_processing_ = true;
        std::string message = incoming_message_queue_.front();
        incoming_message_queue_.pop();

        // Mostrar la IP de destino y el mensaje recibido
        std::cout << "Processing message from " << server_ip_  << std::endl;

        //**************DEBUG********************   std::cout << "Processing message from " << server_ip_ << ": " << message << std::endl;

        if (message.find("$PING") != std::string::npos) {
            std::cout << "PING received from " << server_ip_ << ". Sending PONG..." << std::endl;
            queue_message(Get_Pong_Message());  // Enviar $PONG si se recibe $PING
        }
        else if (message.find("$PONG") != std::string::npos) {
            std::cout << "PONG received from " << server_ip_ << std::endl;
            //queue_message(Get_Ping_Message());  // Enviar $PING si se recibe $PONG
        }

        is_processing_ = false;

        if (!incoming_message_queue_.empty()) {
            process_incoming_message();
        }
        else {
            do_read();  // Continuar leyendo más mensajes
        }
    }


    void queue_message(const std::string& message) {
        bool write_in_progress = !message_queue_.empty();
        message_queue_.push(message);
        if (!write_in_progress) {
            write_impl();
        }
    }

    void write_impl() {
        auto self(shared_from_this());
        if (message_queue_.empty()) {
            return;
        }
        is_writing_ = true;
        auto message = message_queue_.front() +"\n"; // Ading \n to all messages
        auto buffer = std::make_shared<std::string>(message);

        boost::asio::async_write(socket_, boost::asio::buffer(*buffer),
            [this, self, buffer](boost::system::error_code ec, std::size_t/*length*/) {
                if (!ec) {
                    std::cout << "Message sent successfully to " << server_ip_ << std::endl;
                    //*********************** DEBUG std::cout << "Message sent successfully to " << server_ip_ << " -> " << *buffer << std::endl;
                    message_queue_.pop();
                    if (!message_queue_.empty()) {
                        write_impl();
                    }
                    else {
                        is_writing_ = false;
                        do_read();  // Start reading after writing
                    }
                }
                else {
                    std::cout << "Error sending message to " << server_ip_ << ": " << ec.message() << std::endl;
                    socket_.close();  // Close socket on error
                }
            });
    }
    
    std::string Get_Presentation_Message()
    {
         // TO-DO
         // Get Server Public IP.

        std::time_t utc_time = std::time(nullptr);
            return protocol + " " +"173.249.18.228" + " 0.4.2Da1" + " " +
            std::to_string(utc_time);
    }
    
    
    std::string Get_Ping_Message() 
    {
        /*TO-DO
            - Generate PING Message with real values*/

        return protocol + " " + std::to_string(version) + " " + mainnet_version + " " +
            std::to_string(utc_time) + " $PING " +
            "1 0 4E8A4743AA6083F3833DDA1216FE3717 D41D8CD98F00B204E9800998ECF8427E 0 " +
            "D41D8CD98F00B204E9800998ECF8427E 0 8080 D41D8 0 " +
            "00000000000000000000000000000000 0 D41D8CD98F00B204E9800998ECF8427E D41D8";

    }

    std::string Get_Pong_Message() {
        /*TO-DO
            - Generate PONG Message with real values*/

        return protocol + " " + std::to_string(version) + " " + mainnet_version + " " +
            std::to_string(utc_time) + " $PONG " +
            "1 0 4E8A4743AA6083F3833DDA1216FE3717 D41D8CD98F00B204E9800998ECF8427E 0 " +
            "D41D8CD98F00B204E9800998ECF8427E 0 8080 D41D8 0 " +
            "00000000000000000000000000000000 0 D41D8CD98F00B204E9800998ECF8427E D41D8";
    }
    
    void do_write(const std::string& message) {
        queue_message(message);  // Send Message to Queue
    }

    void do_read() {
        auto self(shared_from_this());
        auto buffer = std::make_shared<std::string>();
        boost::asio::async_read_until(socket_, boost::asio::dynamic_buffer(*buffer), "\n",
            [this, self, buffer](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    std::string response = buffer->substr(0, length);
                   // std::cout << "Raw response data: " << response << std::endl;   ////DEBUG 
                    SaveLogToFile("ReceivedIp.txt", server_ip_);
                    incoming_message_queue_.push(response);
                    process_incoming_message();
                }
                else {
                    std::cout << "Error reading from " << server_ip_ << ": " << ec.message() << std::endl;
                }
            });
    }



    // Version information
    std::string protocol = "PSK";
    int version = 2;
    std::string mainnet_version = "0.4.2Da1";
    
    //Time 
    
    std::time_t utc_time = std::time(nullptr);

    boost::asio::steady_timer timer_;
    tcp::socket socket_;
    std::string server_ip_;
    std::string psk_;
    std::string data_;

    std::queue<std::string> message_queue_;  // Message Queue
    bool is_writing_;  // Check if there is any message being writed
    bool is_processing_; // Check if Incoming queue is being used
    std::queue<std::string> incoming_message_queue_;
   

  
};



class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, int session_id)
        : socket_(std::move(socket)), session_id_(session_id) {}

    void start() {
        std::cout << "Session from External client" << session_id_ << " started." << std::endl;
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
    int LastBlock = 0;  // Last BLock Validated by Server, checking blocks.chk ( dafault value 0 ).
public:
    Server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), session_counter_(10000) {
        do_accept();
    }

    void Initialize(boost::asio::io_context& io_context) {
        std::cout << "Initializing Node" << std::endl;
        UpdateSeedTestnetIpServers();
        ConnectToSeedServers(io_context); // Connect to all Seed Servers
        CheckNosoBlocks();
    }

    void ConnectToSeedServers(boost::asio::io_context& io_context) {
        for (const auto& ip : SeedIpAddresses) {
            auto connection = std::make_shared<SeedConnection>(io_context, ip);
            connection->start();  // Start the connection for each IP
        }
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
        //int LastBlock = 0;
        std::cout << "Checking Node BLocks "  << std::endl;
        //READ LastBlock Variable ?
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
            std::cout << "File blocks.chk already exists, loading data to LastBlock: " << LastBlock << std::endl;
          
        }

        std::ifstream infile(blocks_chk_file);
        if (infile.is_open()) {
            infile >> LastBlock;
            std::cout << "Loaded LastBlock value: " << LastBlock << std::endl;
        }
        else {
            std::cerr << "Error: Could not open file blocks.chk for reading." << std::endl;
        }

        if (LastBlock == 0)
        {
            // Open AO Connection to all Seed Servers, Send presentation string and then $LASTBLOCK command to download.
            // Create a Vector with Seed Node Connections ? //Connect to Seed Servers ?
            // Use SeedIpAddresses or TestnetSeedIpAdresses vector.
            // std::vector<std::string> SeedIpAddresses;
            // std::vector<std::string> TestnetSeedIpAddresses;
            // If LastBLock = 0, full download, connect to several seed servers to balance download from the Seed Vector or Testnet Vector if using tesnet ?
            // Select from Seed Servers, and open connection to spread download , checking how many seed servers on vector, and send 100 packages divided by seed 
            // server, every time a packge is downloaded, un zip all .blk files and check headers to validate this block, update block.chk with blocks validated.
            // Store all blocks on NOSODATA/BLOCKS and update Last Block variable.

            // If Lastblock !=0, check what blocks are needed to download, in 100 packs.
        }
        else
        {
            //Logic to download specific blocks.


        }
    }

    void UpdateBlocksChkFile(int last_block) {  //Function to Update BLocks.chk with last block validated.
        const std::filesystem::path blocks_chk_file = "NOSODATA/BLOCKS/blocks.chk";
        std::ofstream outfile(blocks_chk_file);
        if (outfile.is_open()) {
            outfile << last_block;
            outfile.close();
            std::cout << "blocks.chk updated to LastBlock: " << last_block << std::endl;
        }
        else {
            std::cerr << "Error: Could not update file blocks.chk." << std::endl;
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
        
        if (!UseTestnet) {
            std::cout << "Starting Server on Port " << port << std::endl;
            Server server(io_context, port);
            server.Initialize(io_context); // Start doing server initial checks and setup before going online.
            io_context.run();
        }
        else {
            std::cout << "Starting Server on TESTNET Port " << testnetPort << std::endl;
            Server server(io_context, testnetPort);
            server.Initialize(io_context); // Start doing server initial checks and setup before going online.
            io_context.run();
        }
  
        
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
