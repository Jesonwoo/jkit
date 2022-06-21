#ifndef H264PARSER_H
#define H264PARSER_H
#include "../common.h"
#include "H264StreamInfo.h"
#include "Nalu.h"
namespace jkit {
class H264ParserPrivate;
using namespace std;

class MEDIA_EXPORT H264Parser
{
public:
    H264Parser(const std::string &filepath);
    bool open(void);
    bool close(void);
    uint64_t getNaluCount(void);
    bool getNaluInfo(uint64_t idx, Nalu &nalu);
    bool getStreamInfo(H264StreamInfo &info);
    string getNaluSyntax(uint64_t idx);

    string filepath(void);
    // TEST
    void showNalus(void);
    void showStreamInfo(void);

private:
    shared_ptr<H264ParserPrivate> m_d;
};
} // namespace jkit

#endif // H264PARSER_H
