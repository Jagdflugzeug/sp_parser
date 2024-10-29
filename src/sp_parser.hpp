#ifndef SIMPLE_PARSER_HPP
#define SIMPLE_PARSER_HPP

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>
#include <string>



const uint8_t POLL = 0x60;
const uint8_t BUZ = 0x6A;
const uint8_t INIT_BYTE = 0x53;

const size_t CODE_COMMAND_POSITION = 6;
const size_t POLL_COMMAND_SIZE = 7;
const size_t BUZ_COMMAND_SIZE = 12;
const size_t MAX_BUFFER_SIZE = 255;

class Packet
{
public:
    virtual std::string to_string() const = 0;

};


class PollPacket : public Packet{
    std::string type;
    uint8_t addr;
    uint8_t sqn;

public:
    PollPacket(std::string type, uint8_t addr, uint8_t sqn) : type{std::move(type)}, addr{addr}, sqn{sqn}
    {}
    std::string to_string() const override;

};




class BuzPacket : public Packet{
    std::string type;
    uint8_t addr;
    uint8_t sqn;
    uint8_t reader;
    uint8_t tone;
    uint8_t on;
    uint8_t off;
    uint8_t count;

public:
    BuzPacket(std::string type, uint8_t addr, uint8_t sqn, uint8_t reader, uint8_t tone, uint8_t on, uint8_t off,
              uint8_t count) :
    type{std::move(type)}, addr{addr}, sqn{sqn}, reader{reader}, tone{tone}, on{on}, off{off}, count{count}
    {}

    std::string to_string() const override;


};



class SimpleParser
{
public:
    void push(uint8_t b);
    void reset();
    std::shared_ptr<Packet> get_packet() const;
private:
    std::vector<uint8_t>buffer;
    std::shared_ptr<Packet> packet = nullptr;
    uint8_t expected_command = 0x0;
    uint8_t checksum;
    bool is_initialized = false;
    unsigned byte_count = 0;

};

#endif // SIMPLE_PARSER_HPP