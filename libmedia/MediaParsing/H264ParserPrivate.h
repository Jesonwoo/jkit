#ifndef H264PARSERPRIVATE_H
#define H264PARSERPRIVATE_H
#include "../libmedia.h"
#include "Nalu.h"
#include "H264StreamInfo.h"
#include "h264bitstream/h264_stream.h"

namespace jkit {
using namespace std;
class H264StreamInfo;
class H264ParserPrivate
{
public:
    H264ParserPrivate(const string &filepath);
    bool open(void);
    bool close(void);
    uint64_t getNaluCount(void);
    bool getNaluInfo(uint64_t idx, Nalu &nalu);
    string getNaluSyntax(uint64_t idx);
    bool getStreamInfo(H264StreamInfo &info);

    inline string filepath(void) { return m_filepath; }

    // TEST
    void showNalus(void);
    void showStreamInfo(void);

private:
    void getH264StreamIfno(h264_stream_t *s, H264StreamInfo&info);
    void getResolution(h264_stream_t *s, int32_t &width, int32_t &height);
    float getMaxFrameRate(h264_stream_t *s);
    string getChromaFormat(h264_stream_t *s);
    string getProfile(h264_stream_t *s);
    float getLevel(h264_stream_t *s);

    // 引用h264bstream中的代码
    string getSyntax(h264_stream_t *s);
    string getSPSSyntax(sps_t* sps);
    string getPPSSyntax(pps_t* pps);
    string getSliceHeaderSyntax(slice_header_t* sh);
    string getAudSyntax(aud_t* aud);
    string getSEISyntax(h264_stream_t *h);
    string hexToString(const uint8_t *buf, size_t size, int newline = -1);

private:
    bool m_isInit = false;
    int32_t m_sliceNum = 0;
    int64_t m_frameCount = 0;
    string m_filepath;
    H264StreamInfo m_streamInfo;
    vector<Nalu> m_naluList;
};
} // namespace jkit

#endif // H264PARSERPRIVATE_H
