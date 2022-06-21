#include "NaluPrivate.h"
namespace jkit{

void NaluPrivate::debugNalu()
{
    printf("========================================\n");
    printf("offset:     0x%08X(%llu)\n", m_offset, m_offset);
    printf("frame num:  %lld\n", m_frameNum);
    printf("frame num:  %lld\n", m_sliceNum);
    printf("nalu type:  %s\n", Nalu::nalTypeToStr(m_naluType).c_str());
    printf("slice type: %s\n", Nalu::sliceTypeToStr(m_sliceType).c_str());
    printf("start code: %d\n", m_startCodeLength);
    printf("length:     %d\n", m_length);

    fflush(stdout);
}

NaluPrivate::NaluPrivate()
{

}

NaluPrivate::~NaluPrivate()
{

}

NaluPrivate::NaluPrivate(const NaluPrivate & other)
{
    m_naluType = other.m_naluType;
    m_sliceType = other.m_sliceType;
    m_sliceNum = other.m_sliceNum;
    m_length = other.m_length;
    m_startCodeLength = other.m_startCodeLength;
    m_offset = other.m_offset;
    m_frameNum = other.m_frameNum;
}

NaluPrivate::NaluPrivate(NaluPrivate &&other)
{
    m_naluType = std::move(other.m_naluType);
    m_sliceType = std::move(other.m_sliceType);
    m_sliceNum = std::move(other.m_sliceNum);
    m_length = std::move(other.m_length);
    m_startCodeLength = std::move(other.m_startCodeLength);
    m_offset = std::move(other.m_offset);
    m_frameNum = std::move(other.m_frameNum);
}

NaluPrivate &NaluPrivate::operator=(const NaluPrivate &other)
{
    if(this == &other) {
        return *this;
    }
    m_naluType = other.m_naluType;
    m_sliceType = other.m_sliceType;
    m_sliceNum = other.m_sliceNum;
    m_length = other.m_length;
    m_startCodeLength = other.m_startCodeLength;
    m_offset = other.m_offset;
    m_frameNum = other.m_frameNum;
    return *this;
}

NaluPrivate &NaluPrivate::operator=(NaluPrivate &&other)
{
    if(this == &other) {
        return *this;
    }
    m_naluType = std::move(other.m_naluType);
    m_sliceType = std::move(other.m_sliceType);
    m_sliceNum = std::move(other.m_sliceNum);
    m_length = std::move(other.m_length);
    m_startCodeLength = std::move(other.m_startCodeLength);
    m_offset = std::move(other.m_offset);
    m_frameNum = std::move(other.m_frameNum);
    return *this;
}

NaluType NaluPrivate::naluType() const
{
    return m_naluType;
}

void NaluPrivate::setNaluType(NaluType newNaluType)
{
    m_naluType = newNaluType;
}

SliceType NaluPrivate::sliceType() const
{
    return m_sliceType;
}

void NaluPrivate::setSliceType(SliceType newSliceType)
{
    m_sliceType = newSliceType;
}

int32_t NaluPrivate::length() const
{
    return m_length;
}

void NaluPrivate::setLength(int32_t newLength)
{
    m_length = newLength;
}

int32_t NaluPrivate::startCodeLength() const
{
    return m_startCodeLength;
}

void NaluPrivate::setStartCodeLength(int32_t newStartCodeLength)
{
    m_startCodeLength = newStartCodeLength;
}

int64_t NaluPrivate::offset() const
{
    return m_offset;
}

void NaluPrivate::setOffset(int64_t newOffset)
{
    m_offset = newOffset;
}

int64_t NaluPrivate::frameNum() const
{
    return m_frameNum;
}

void NaluPrivate::setFrameNum(int64_t newFrameNum)
{
    m_frameNum = newFrameNum;
}

int32_t NaluPrivate::sliceNum() const
{
    return m_sliceNum;
}

void NaluPrivate::setSliceNum(int32_t newSliceNum)
{
    m_sliceNum = newSliceNum;
}

} // namespace jkit
