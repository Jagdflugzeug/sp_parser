#include "sp_parser.hpp"
#include <algorithm>



uint8_t getchecksum(std::vector<uint8_t>::const_iterator begin, std::vector<uint8_t>::const_iterator end)
{
    int whole_checksum = 0;
    uint8_t checksum;

    std::for_each(begin, end, [&whole_checksum, &checksum] (uint8_t n) {
        whole_checksum = whole_checksum + n;
        checksum = ~(0xFF & whole_checksum) + 1;
    });

    return checksum;
}

std::shared_ptr<Packet> SimpleParser::get_packet() const {

    return packet;

}

std::string PollPacket::to_string() const {
    std::string res = "type:" + type +
                  ",addr:" + std::to_string(addr) +
                  ",sqn:" + std::to_string(sqn);

return res;
}

std::string BuzPacket::to_string() const {

    std::string res = "type:" + type + ",addr:" +
                  std::to_string(addr) + ",sqn:" +
                  std::to_string(sqn) + ",reader:" +
                  std::to_string(reader) + ",tone:" +
                  std::to_string(tone) + ",on:" +
                  std::to_string(on) + ",off:" +
                  std::to_string(off) + ",count:"+
                  std::to_string(count);

return res;
}


void SimpleParser::push(uint8_t byte) {
        if (byte == INIT_BYTE){
            is_initialized = true;
            byte_count = 0;
            buffer.clear();
        }
        if (!is_initialized)
            return;

        if (buffer.size() >= MAX_BUFFER_SIZE){
            //error
            reset();
            return;
        }
        byte_count += 1;
        buffer.push_back(byte);

        if (byte_count == CODE_COMMAND_POSITION){
            expected_command = buffer.at(CODE_COMMAND_POSITION - 1);
        }
        if (expected_command == POLL && buffer.size() == POLL_COMMAND_SIZE){
            checksum = getchecksum(buffer.cbegin(), std::prev(buffer.cend()));
            if (checksum == byte) {
                packet = std::make_shared<PollPacket>("POLL", buffer.at(1),
                                                      buffer.at(4));
            }
            else{
                packet = nullptr;
            }

            reset();
            return;
        }
        if (expected_command == BUZ && buffer.size() == BUZ_COMMAND_SIZE) {
            checksum = getchecksum(buffer.cbegin(), std::prev(buffer.cend()));
            if (checksum == byte) {
                packet = std::make_shared<BuzPacket>("BUZ", buffer.at(1), //addr
                                                     buffer.at(4), //sqn
                                                     buffer.at(6), //reader
                                                     buffer.at(7), //tone
                                                     buffer.at(8), //on
                                                     buffer.at(9), //off
                                                     buffer.at(10)); //count
            }
            else{
                packet = nullptr;
            }

            reset();
            return;
        }
}

void SimpleParser::reset() {
    expected_command = 0;
    buffer.clear();
    is_initialized = false;
}



