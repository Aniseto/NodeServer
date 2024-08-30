#pragma once



#include <botan/hash.h>
#include <botan/hex.h>
#include <string>
#include <iostream>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <botan/auto_rng.h>
#include <botan/rsa.h>
#include <botan/base58.h>
#include <botan/ecdsa.h>
#include <botan/pubkey.h>
#include <botan/bigint.h>
#include <botan/base64.h>
#include <botan/hex.h>
#include <cctype>
#include <botan/hash.h>
#include <filesystem>
#include <ctime>
#include <botan/auto_rng.h>
#include <botan/ecdsa.h>
#include <botan/hex.h>
#include <botan/bigint.h>
#include <botan/base58.h>
#include <botan/base64.h>
#include <botan/hex.h>

#include <botan/pubkey.h>
#include <botan/ec_group.h>
#include <botan/ecdsa.h>
#include <botan/auto_rng.h>
#include <botan/base64.h>
#include <botan/p11_ecdsa.h>
#include <botan/pem.h>

#include <botan/pkcs8.h>
#include <botan/pk_keys.h>
#include <iostream>
#include <vector>
#include <bitset>

const std::string B64Alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const std::string B58Alphabet = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
const std::string B36Alphabet = "0123456789abcdefghijklmnopqrstuvwxyz";
std::string HexAlphabet = "0123456789ABCDEF";

/* To-DO
    Crypto functions to generate a NOSO addreess.*/

std::string CalculateRIPEMD160_Botan(const std::string& input)
{
    std::unique_ptr<Botan::HashFunction> hash(Botan::HashFunction::create("RIPEMD-160"));
    hash->update(reinterpret_cast<const uint8_t*>(input.data()), input.size());
    Botan::secure_vector<uint8_t> digest = hash->final();  // Usar secure_vector en lugar de std::vector
    return Botan::hex_encode(digest);
}

std::string getHashSha256ToString(const std::string& publicKey)
{
    // Crear el objeto SHA-256 utilizando HashFunction
    std::unique_ptr<Botan::HashFunction> sha256(Botan::HashFunction::create("SHA-256"));

    // Actualizar el objeto SHA-256 con los datos de la clave pública
    sha256->update(reinterpret_cast<const uint8_t*>(publicKey.data()), publicKey.length());

    // Obtener el hash final como un secure_vector
    Botan::secure_vector<uint8_t> digest = sha256->final();

    // Convertir el hash a una cadena hexadecimal
    std::string result = Botan::hex_encode(digest);

    // Convertir el resultado a mayúsculas
    for (char& c : result)
    {
        if (c == '-')
            c = ' ';
        c = std::toupper(c);
    }

    return result;
}

std::string EncodeBase58(const std::string& MD160String)
{

    std::vector<uint8_t> inputData = Botan::hex_decode(MD160String);

    std::string base58Result = Botan::base58_encode(inputData.data(), inputData.size());

    return base58Result;

}

int CalculateCheckSum(const std::string& StringChecksum)

{

    int total = 0;

    for (int counter = 0; counter < StringChecksum.length(); ++counter) {
        total += B58Alphabet.find(StringChecksum[counter]);
    }

    return total;

}

std::string BmDecto58(const std::string& number)
{


    std::string decimalValue = number;
    std::string result = "";

    while (decimalValue.length() >= 2) {
        int restante;
        int divisionResult = std::stoi(decimalValue) / 58;
        restante = std::stoi(decimalValue) - divisionResult * 58;
        decimalValue = std::to_string(divisionResult);
        result = B58Alphabet[restante] + result;
    }

    if (std::stoi(decimalValue) >= 58) {
        int restante;
        int divisionResult = std::stoi(decimalValue) / 58;
        restante = std::stoi(decimalValue) - divisionResult * 58;
        decimalValue = std::to_string(divisionResult);
        result = B58Alphabet[restante] + result;
    }

    if (std::stoi(decimalValue) > 0) {
        result = B58Alphabet[std::stoi(decimalValue)] + result;
    }

    return result;
}

//    NosoAddress = "N" + Base58 + CheckSumBase58;