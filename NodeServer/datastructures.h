#pragma once
class NodeStatusData {

    //NODESTATUS 1{Peers} 2{LastBlock} 3{Pendings} 4{Delta} 5{headers} 6{version} 7{UTCTime} 8{MNsHash}
    //           9{MNscount} 10{LasBlockHash} 11{BestHashDiff} 12{LastBlockTimeEnd} 13{LBMiner}
    //           14{ChecksCount} 15{LastBlockPoW} 16{LastBlockDiff} 17{summary} 18{GVTs} 19{nosoCFG}
    //           20{PSOHash}*/

public:
    std::string NodeStatus = "Empty";             // 0- Saves NODESTATUS Message, just to have control
    unsigned int Peers{ 0 };                       // 1- Saves Number of Peers connected to this Node.
    unsigned int BlockNumber{ 0 };                 // 2- Saves Current Block Number
    unsigned int Pending{ 0 };                     // 3- Number of Pending Operations
    unsigned int Delta{ 0 };                       // 4- Number of Pending Operations
    std::string Headers = "Empty";             // Number of Pending Operations
    std::string Version = "Empty";        // Current Node Version Software
    unsigned int UTCTIme{ 0 };
    std::string MNsHash = "Empty";
    unsigned int MNsCount{ 0 };
    //int Updated {0};
    std::string LastBlockHash = "Empty";
    std::string BestHashDiff = "Empty";
    int LastBlockTimeEnd{ 0 };
    std::string LastBLockMiner = "Empty";
    std::string ChecksCount = "Empty";
    std::string LastBlockPoW = "Empty";
    std::string LastBlockDiff = "Empty";
    std::string Summary = "Empty";
    std::string GVTHash = "Empty";
    std::string NosoCFG = "Empty";
    std::string PSOHash = "Empty";

    //Getters
    std::string GetNodeStatus() { return NodeStatus; }
    unsigned int GetPeers() { return Peers; }
    unsigned int GetBlockNumber() { return BlockNumber; }
    unsigned int GetPending() { return Pending; }
    unsigned int GetDelta() { return Delta; }
    std::string GetHeaders() { return Headers; }
    std::string GetVersion() { return Version; }
    unsigned int GetUTCTIme() { return UTCTIme; }
    std::string GetMNsHash() { return MNsHash; }
    unsigned int GetMNsCount() { return MNsCount; }
    std::string GetLastBlockHash() { return LastBlockHash; }
    std::string GetBestHashDiff() { return BestHashDiff; }
    int GetLastBlockTimeEnd() { return LastBlockTimeEnd; }
    std::string GetLastBLockMiner() { return LastBLockMiner; }
    std::string GetChecksCount() { return ChecksCount; }
    std::string GetLastBlockPoW() { return LastBlockPoW; }
    std::string GetLastBlockDiff() { return LastBlockDiff; }
    std::string GetSummary() { return Summary; }
    std::string GetGVTHash() { return GVTHash; }
    std::string GetNosoCFG() { return NosoCFG; }
    std::string GetPSOHash() { return PSOHash; }
    //Setters

    void SetNodeStatus(std::string NodeStatus) { this->NodeStatus = NodeStatus; }
    void SetPeers(unsigned int Peers) { this->Peers = Peers; }
    void SetBlockNumber(unsigned int BlockNumber) { this->BlockNumber = BlockNumber; }
    void SetPending(unsigned int Pending) { this->Pending = Pending; }
    void SetDelta(unsigned int Delta) { this->Delta = Delta; }
    void SetHeaders(std::string Headers) { this->Headers = Headers; }
    void SetVersion(std::string Version) { this->Version = Version; }
    void SetUTCTIme(unsigned int UTCTIme) { this->UTCTIme = UTCTIme; }
    void SetMNsHash(std::string MNsHash) { this->MNsHash = MNsHash; }

    //Getters
    void SetMNsCount(unsigned int MNsCount) { this->MNsCount = MNsCount; }
    void SetLastBlockHash(std::string LastBlockHash) { this->LastBlockHash = LastBlockHash; }
    void SetBestHashDiff(std::string BestHashDiff) { this->BestHashDiff = BestHashDiff; }
    void SetLastBlockTimeEnd(int LastBlockTimeEnd) { this->LastBlockTimeEnd = LastBlockTimeEnd; }
    void SetLastBLockMiner(std::string LastBLockMiner) { this->LastBLockMiner = LastBLockMiner; }
    void SetChecksCount(std::string ChecksCount) { this->ChecksCount = ChecksCount; }
    void SetLastBlockPoW(std::string LastBlockPoW) { this->LastBlockPoW = LastBlockPoW; }
    void SetLastBlockDiff(std::string LastBlockDiff) { this->LastBlockDiff = LastBlockDiff; }
    void SetSummary(std::string Summary) { this->Summary = Summary; }
    void SetGVTHash(std::string GVTHash) { this->GVTHash = GVTHash; }
    void SetNosoCFG(std::string NosoCFG) { this->NosoCFG = NosoCFG; }
    void SetPSOHash(std::string PSOHash) { this->PSOHash = PSOHash; }



    



};


#pragma pack(push, 1)
struct TSummaryData {
    char empty[1];     // Added because sumary.psk fine is created by a Pascal app that adds an initial Char specifying Numer of Chars
    char Hash[40];     // Public hash (including null terminator)
    char empty2[1];    // Added because sumary.psk fine is created by a Pascal app that adds an initial Char specifying Numer of Chars
    char Custom[40];   // Custom alias (including null terminator)
    int64_t Balance;   // Noso balance
    int64_t Score;     // Token balance
    int64_t LastOP;    // Last operation block
};
#pragma pack(pop)

#pragma pack(push, 1)




class WalletData {

    // This structure is needed in order to allow read summary file fron Pascal Nodes, and maitain backguard compatibility.
    // Can be improved using Strings in a future.

private:
    char Empty[1]; //Due pascal original file
    char Hash[40]; // Public address HashEl hash publico o direccion
    char Empty2[1]; //Due Pascal original file
    char Custom[40]; // Custom Name
    char Empty3[1];  //Due Pascal original file
    char PublicKey[255]; // Public Key
    char Empty4[1]; // Due pascal original file
    char PrivateKey[255]; // Private Key
    int64_t Balance; // Last balance from this address
    int64_t Pending; // Last pending payment
    int64_t Score; // Address status.
    int64_t LastOP;// Time from the last operation in UnixTime Format
public:
    // Getter para obtener el valor de privateKey
    std::string GetPrivateKey() {

        return PrivateKey;
    }
    std::string GetPublicKey() {
        return PublicKey;
    }
    std::string GetHash() {
        return Hash;
    }
    std::string GetLabel() {
        return Custom;
    }
    std::int64_t GetPending() {
        return Pending;
    }
    std::int64_t GetBalance() {
        return Balance;
    }
    //Setters
    void SetPrivateKey(const std::string& privateKey) {

        size_t copySize = std::min(privateKey.size(), sizeof(PrivateKey) - 1);

        // Copiar los datos manualmente
        for (size_t i = 0; i < copySize; ++i) {
            PrivateKey[i] = privateKey[i];
        }
        PrivateKey[copySize] = '\0';
    }
    void SetPublicKey(const std::string& publicKey) {

        size_t copySize = std::min(publicKey.size(), sizeof(PublicKey) - 1);

        // Copiar los datos manualmente
        for (size_t i = 0; i < copySize; ++i) {
            PublicKey[i] = publicKey[i];
        }
        PublicKey[copySize] = '\0';
    }
    void SetHash(const std::string& hash) {

        size_t copySize = std::min(hash.size(), sizeof(Hash) - 1);

        // Copiar los datos manualmente
        for (size_t i = 0; i < copySize; ++i) {
            Hash[i] = hash[i];
        }
        Hash[copySize] = '\0';
    }
    void SetCustom(const std::string& custom) {

        size_t copySize = std::min(custom.size(), sizeof(Custom) - 1);

        // Copiar los datos manualmente
        for (size_t i = 0; i < copySize; ++i) {
            Custom[i] = custom[i];
        }
        Custom[copySize] = '\0';
    }
    void SetBalance(int64_t balance) {

        Balance = balance;
    }
    void SetPending(int64_t pending) {

        Pending = pending;
    }
    void SetScore(int64_t score) {

        Score = score;
    }
    void SetLastOp(int64_t lastop) {

        LastOP = lastop;
    }


};
#pragma pack(pop)
