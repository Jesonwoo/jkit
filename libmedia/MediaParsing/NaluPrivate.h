#ifndef NALUPRIVATE_H
#define NALUPRIVATE_H
#include "Nalu.h"
namespace jkit{
class NaluPrivate
{
public:
    NaluPrivate();
    ~NaluPrivate();

    NaluPrivate(const NaluPrivate &other);
    NaluPrivate(NaluPrivate &&other);
    NaluPrivate &operator=(const NaluPrivate &other);
    NaluPrivate &operator=(NaluPrivate &&other);

    NaluType naluType() const;
    void setNaluType(NaluType newNaluType);

    SliceType sliceType() const;
    void setSliceType(SliceType newSliceType);

    int32_t length() const;
    void setLength(int32_t newLength);

    int32_t startCodeLength() const;
    void setStartCodeLength(int32_t newStartCodeLength);

    int64_t offset() const;
    void setOffset(int64_t newOffset);

    int64_t frameNum() const;
    void setFrameNum(int64_t newFrameNum);

    // TEST
    void debugNalu();

private:
    NaluType  m_naluType = NALU_TYPE_UNSPECIFIED;
    SliceType m_sliceType = SLICE_TYPE_UNKOWN;

    int32_t m_length = 0;     // payload+startcode
    int32_t m_startCodeLength = 4;
    int64_t m_offset = 0;    // 文件中的偏移位置
    int64_t m_frameNum = 0;  // 帧编号[0, n)
};
} // namespace jkit
#endif // NALUPRIVATE_H
