#ifndef NALU_H
#define NALU_H
#include "../common.h"
namespace jkit{
enum NaluType {
    //Table 7-1 NAL unit type codes
    NALU_TYPE_UNSPECIFIED                  =  0,    // Unspecified
    NALU_TYPE_CODED_SLICE_NON_IDR          =  1,    // Coded slice of a non-IDR picture
    NALU_TYPE_CODED_SLICE_DATA_PARTITION_A =  2,    // Coded slice data partition A
    NALU_TYPE_CODED_SLICE_DATA_PARTITION_B =  3,    // Coded slice data partition B
    NALU_TYPE_CODED_SLICE_DATA_PARTITION_C =  4,    // Coded slice data partition C
    NALU_TYPE_CODED_SLICE_IDR              =  5,    // Coded slice of an IDR picture
    NALU_TYPE_SEI                          =  6,    // Supplemental enhancement information (SEI)
    NALU_TYPE_SPS                          =  7,    // Sequence parameter set
    NALU_TYPE_PPS                          =  8,    // Picture parameter set
    NALU_TYPE_AUD                          =  9,    // Access unit delimiter
    NALU_TYPE_END_OF_SEQUENCE              = 10,    // End of sequence
    NALU_TYPE_END_OF_STREAM                = 11,    // End of stream
    NALU_TYPE_FILLER                       = 12,    // Filler data
    NALU_TYPE_SPS_EXT                      = 13,    // Sequence parameter set extension
                                    // 14...18,    // Reserved
    NALU_TYPE_CODED_SLICE_AUX              = 19,    // Coded slice of an auxiliary coded picture without partitioning
                                    // 20..23      // Reserved
                                    // 24..31      // Unspecified
};

enum SliceType {
    SLICE_TYPE_UNKOWN   =   -1,        // unkown
    //7.4.3 Table 7-6. Name association to slice_type
    SLICE_TYPE_P        =    0,        // P (P slice)
    SLICE_TYPE_B        =    1,        // B (B slice)
    SLICE_TYPE_I        =    2,        // I (I slice)
    SLICE_TYPE_SP       =    3,        // SP (SP slice)
    SLICE_TYPE_SI       =    4,        // SI (SI slice)
    //as per footnote to Table 7-6, the *_ONLY
    //slice types indicate that all other slices in that picture are of the same type
    SLICE_TYPE_P_ONLY   =    5,        // P (P slice)
    SLICE_TYPE_B_ONLY   =    6,        // B (B slice)
    SLICE_TYPE_I_ONLY   =    7,        // I (I slice)
    SLICE_TYPE_SP_ONLY  =    8,        // SP (SP slice)
    SLICE_TYPE_SI_ONLY  =    9,        // SI (SI slice)
};

class NaluPrivate;
class MEDIA_EXPORT Nalu
{
public:
    Nalu();
    ~Nalu();
    Nalu(const Nalu &other);
    Nalu(Nalu &&other);
    Nalu &operator=(const Nalu &other);
    Nalu &operator=(Nalu &&other);

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

    static std::string nalTypeToStr(NaluType type);
    static std::string sliceTypeToStr(SliceType type);

    // TEST
    void debugNalu();

private:
    std::shared_ptr<NaluPrivate> m_d;
};
} // namespace jkit
#endif // NALU_H
