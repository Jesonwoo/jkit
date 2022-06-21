#include "Nalu.h"
#include "NaluPrivate.h"
namespace jkit{

void Nalu::debugNalu()
{
    m_d->debugNalu();
}

Nalu::Nalu()
    : m_d(new NaluPrivate)
{
}

Nalu::~Nalu()
{
}

Nalu::Nalu(const Nalu & other)
    : Nalu()
{
    *m_d = *other.m_d;
}

Nalu::Nalu(Nalu &&other)
    : Nalu()
{
    *m_d = *other.m_d;
}

Nalu &Nalu::operator=(const Nalu &other)
{
    if(this == &other) {
        return *this;
    }
    *m_d = *other.m_d;
    return *this;
}

Nalu &Nalu::operator=(Nalu &&other)
{
    if(this == &other) {
        return *this;
    }
    *m_d = std::move(*other.m_d);
    return *this;
}

NaluType Nalu::naluType() const
{
    return m_d->naluType();
}

void Nalu::setNaluType(NaluType newNaluType)
{
    m_d->setNaluType(newNaluType);
}

SliceType Nalu::sliceType() const
{
    return m_d->sliceType();
}

void Nalu::setSliceType(SliceType newSliceType)
{
    m_d->setSliceType(newSliceType);
}

int32_t Nalu::length() const
{
    return m_d->length();
}

void Nalu::setLength(int32_t newLength)
{
    m_d->setLength(newLength);
}

int32_t Nalu::startCodeLength() const
{
    return m_d->startCodeLength();
}

void Nalu::setStartCodeLength(int32_t newStartCodeLength)
{
    m_d->setStartCodeLength(newStartCodeLength);
}

int64_t Nalu::offset() const
{
    return m_d->offset();
}

void Nalu::setOffset(int64_t newOffset)
{
    m_d->setOffset(newOffset);
}

int64_t Nalu::frameNum() const
{
    return m_d->frameNum();
}

void Nalu::setFrameNum(int64_t newFrameNum)
{
    m_d->setFrameNum(newFrameNum);
}

int32_t Nalu::sliceNum() const
{
    return m_d->sliceNum();
}

void Nalu::setSliceNum(int32_t newSliceNum)
{
    m_d->setSliceNum(newSliceNum);
}

std::string Nalu::nalTypeToStr(NaluType type)
{
    std::string ret;
    switch(type) {
    case NALU_TYPE_UNSPECIFIED:
        ret = "UNSPECIFIED";
        break;
    case NALU_TYPE_CODED_SLICE_NON_IDR:
        ret = "CODED_SLICE_NON_IDR";
        break;
    case NALU_TYPE_CODED_SLICE_DATA_PARTITION_A:
        ret = "CODED_SLICE_DATA_PARTITION_A";
        break;
    case NALU_TYPE_CODED_SLICE_DATA_PARTITION_B:
        ret = "CODED_SLICE_DATA_PARTITION_B";
        break;
    case NALU_TYPE_CODED_SLICE_DATA_PARTITION_C:
        ret = "CODED_SLICE_DATA_PARTITION_C";
        break;
    case NALU_TYPE_CODED_SLICE_IDR:
        ret = "CODED_SLICE_IDR";
        break;
    case NALU_TYPE_SEI:
        ret = "SEI";
        break;
    case NALU_TYPE_SPS:
        ret = "SPS";
        break;
    case NALU_TYPE_PPS:
        ret = "PPS";
        break;
    case NALU_TYPE_AUD:
        ret = "AUD";
        break;
    case NALU_TYPE_END_OF_SEQUENCE:
        ret = "END_OF_SEQUENCE";
        break;
    case NALU_TYPE_END_OF_STREAM:
        ret = "END_OF_STREAM";
        break;
    case NALU_TYPE_FILLER:
        ret = "FILLER";
        break;
    case NALU_TYPE_SPS_EXT:
        ret = "SPS_EXT";
        break;
    case NALU_TYPE_CODED_SLICE_AUX:
        ret = "CODED_SLICE_AUX";
        break;
    default:
        ret = "UNSPECIFIED";
        break;
    }
    return ret;
}

std::string Nalu::sliceTypeToStr(SliceType type)
{
    std::string ret;
    switch (type) {
    case SLICE_TYPE_P  :        // P (P slice)
        ret = "P";
        break;
    case SLICE_TYPE_B  :        // B (B slice)
        ret = "B";
        break;
    case SLICE_TYPE_I  :        // I (I slice)
        ret = "I";
        break;
    case SLICE_TYPE_SP :        // SP (SP slice)
        ret = "P";
        break;
    case SLICE_TYPE_SI :        // SI (SI slice)
        ret = "I";
        break;
    //as per footnote to Table 7-6, the *_ONLY
    //slice types indicate that all other slices in that picture are of the same type
    case SLICE_TYPE_P_ONLY  :   // P (P slice)
        ret = "P";
        break;
    case SLICE_TYPE_B_ONLY  :   // B (B slice)
        ret = "B";
        break;
    case SLICE_TYPE_I_ONLY  :   // I (I slice)
        ret = "I";
        break;
    case SLICE_TYPE_SP_ONLY :   // SP (SP slice)
        ret = "P";
        break;
    case SLICE_TYPE_SI_ONLY :   // SI (SI slice)
        ret = "I";
        break;
    default:
        ret = "Unkown";
        break;
    }
    return ret;
}

} // namespace jkit
