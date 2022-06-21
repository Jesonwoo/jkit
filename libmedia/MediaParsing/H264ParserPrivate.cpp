#include "H264ParserPrivate.h"
namespace jkit{
using namespace std;
#define LOGE(t, ...) \
    do{fprintf(stderr, __VA_ARGS__); fflush(stderr);}while(0)

#define PRINT_TO_SYNTAX(syntax, ...) \
    do{ char buf[1024] = {0}; sprintf(buf, __VA_ARGS__); syntax.append(buf); }while(0);
H264ParserPrivate::H264ParserPrivate(const std::string &filepath)
    : m_filepath(filepath)
{
}

bool H264ParserPrivate::open()
{
    if(m_isInit) {
        return true;
    }
    constexpr int bufMaxSize = 32*1024*1024;
    h264_stream_t *h264Stream = h264_new();
    uint8_t * buf = nullptr;

    fstream stream(m_filepath, fstream::in | fstream::binary);
    if(!stream.is_open()) {
        LOGE(this, "Failed to open [%s]\n", m_filepath.c_str());
        return false;
    }
    buf = new(std::nothrow) uint8_t[bufMaxSize];
    if(nullptr == buf) {
        LOGE(this, "Failed to apply for buffer[size:%d]!!\n", bufMaxSize);
        stream.close();
        return false;
    }
    int byteCount = 0;
    int64_t offset = 0, sz = 0;
    int nalStart; int nalEnd;
    uint8_t* p = buf;

    while(true){
        if((byteCount = stream.read((char*)buf+sz, bufMaxSize-sz).gcount())<=0) {
            break;
        }
        sz += byteCount;
        while(find_nal_unit(p, sz, &nalStart, &nalEnd) > 0) {
            // skip start code
            p += nalStart;
            // got a nalu
            read_nal_unit(h264Stream, p, nalEnd - nalStart);
            // 获取h264流相关信息
            getH264StreamIfno(h264Stream, m_streamInfo);

            Nalu n;
            int nalType = h264Stream->nal->nal_unit_type;
            if(nalType == NALU_TYPE_CODED_SLICE_IDR
                    || nalType == NALU_TYPE_CODED_SLICE_NON_IDR
                    || nalType == NALU_TYPE_CODED_SLICE_AUX) {
                if(h264Stream->sh->first_mb_in_slice == 0) {
                    m_sliceNum = 0;
                } else {
                    m_sliceNum++;
                }
                n.setSliceType((SliceType)h264Stream->sh->slice_type);
            }
            n.setLength(nalEnd);
            n.setStartCodeLength(nalStart);
            n.setSliceNum(m_sliceNum);
            n.setOffset(offset + (p - buf) - nalStart);
            n.setNaluType((NaluType)h264Stream->nal->nal_unit_type);
            n.setFrameNum(m_streamInfo.frameCount()-1);
            m_naluList.push_back(n);

            p += (nalEnd - nalStart);
            sz -= nalEnd;
        }

        if (p == buf) {
            LOGE(this, "!! Did not find any NALs between offset %lld (0x%04llX), size %lld (0x%04llX), discarding \n",
                 offset, offset, offset + sz, offset + sz);
            p = buf + sz;
            sz = 0;
        }
        memmove(buf, p, sz);
        offset += p - buf;
        p = buf;
    }
    m_streamInfo.setDuration((1000/m_streamInfo.frameRate())*m_streamInfo.frameCount());

    fflush(stdout);
    stream.close();
    delete[] buf;
    h264_free(h264Stream);

    m_isInit = true;
    return true;
}

void H264ParserPrivate::getH264StreamIfno(h264_stream_t *s, H264StreamInfo & info)
{
    if(nullptr == s) {
        return;
    }
    switch (s->nal->nal_unit_type)
    {
    case NALU_TYPE_CODED_SLICE_IDR:
    case NALU_TYPE_CODED_SLICE_NON_IDR:
    case NALU_TYPE_CODED_SLICE_AUX:
        if(s->sh->first_mb_in_slice == 0) {
            info.setFrameCount(++m_frameCount);
        }
        break;
    case NALU_TYPE_SEI:
        break;
    case NALU_TYPE_SPS: {
        int32_t w = 0, h = 0;
        getResolution(s, w, h);
        info.setChromaFormat(getChromaFormat(s));
        info.setWidth(w);
        info.setHeigth(h);
        info.setFrameRate(getMaxFrameRate(s));
        info.setLevel(getLevel(s));
        info.setProfile(getProfile(s));
    }
        break;
    case NALU_TYPE_PPS:
        break;
    case NALU_TYPE_AUD:
        break;
    case NALU_TYPE_END_OF_SEQUENCE:
        break;
    case NALU_TYPE_END_OF_STREAM:
        break;
    case NALU_TYPE_FILLER:
    case NALU_TYPE_SPS_EXT:
    case NALU_TYPE_UNSPECIFIED:
    case NALU_TYPE_CODED_SLICE_DATA_PARTITION_A:
    case NALU_TYPE_CODED_SLICE_DATA_PARTITION_B:
    case NALU_TYPE_CODED_SLICE_DATA_PARTITION_C:
    default:
        return;
    }

}

void H264ParserPrivate::getResolution(h264_stream_t *s, int32_t &width, int32_t &height)
{
    if(s->nal->nal_unit_type != NALU_TYPE_SPS){
        return;
    }
    width = (s->sps->pic_width_in_mbs_minus1 + 1)*16;
    height = ((2 - s->sps->frame_mbs_only_flag)* (s->sps->pic_height_in_map_units_minus1 +1) * 16);
    if(s->sps->frame_cropping_flag)  {
        int32_t x;
        int32_t y;
        if (0 == s->sps->chroma_format_idc) {
            x = 1;
            y = 2 - s->sps->frame_mbs_only_flag;
        } else if (1 == s->sps->chroma_format_idc) {
            x = 2;
            y = 2 * (2 - s->sps->frame_mbs_only_flag);
        } else if (2 == s->sps->chroma_format_idc) {
            x = 2;
            y = 2 - s->sps->frame_mbs_only_flag;
        } else {
            x = 1;
            y = 2 - s->sps->frame_mbs_only_flag;
        }
        width  -= x * (s->sps->frame_crop_left_offset + s->sps->frame_crop_right_offset);
        height -= y * (s->sps->frame_crop_top_offset  + s->sps->frame_crop_bottom_offset);
    }
}

float H264ParserPrivate::getMaxFrameRate(h264_stream_t *s)
{
    float rate = 0.0f;
    if(s->nal->nal_unit_type != NALU_TYPE_SPS){
        return rate;
    }
    // 计算帧率
    if(s->sps->vui_parameters_present_flag) {
        if(s->sps->vui.num_units_in_tick != 0) {
            rate = (float)(s->sps->vui.time_scale) / (float)(s->sps->vui.num_units_in_tick)/2;
        }
    }
    return rate;
}

string H264ParserPrivate::getChromaFormat(h264_stream_t *s)
{
    string str;
    if(s->nal->nal_unit_type != NALU_TYPE_SPS){
        return str;
    }
    switch (s->sps->chroma_format_idc) {
    case 0:
        str = "monochrome";
        break;
    case 1:
        str = "4:2:0";
        break;
    case 2:
        str = "4:2:2";
        break;
    case 3:
        str = "4:4:4";
        break;
    default:
        str = "4:2:0";
        break;

    }
    return str;
}

string H264ParserPrivate::getProfile(h264_stream_t *s)
{
    string str;
    if(s->nal->nal_unit_type != NALU_TYPE_SPS){
        return str;
    }
    if(s->sps->profile_idc == H264_PROFILE_BASELINE) {
        str = "BaseLine";
    } else if(s->sps->profile_idc == H264_PROFILE_MAIN) {
        str = "Main";
    } else if(s->sps->profile_idc == H264_PROFILE_EXTENDED) {
        str = "Extended";
    } else if(s->sps->profile_idc == H264_PROFILE_HIGH) {
        str = "High";
    }
    return str;
}

float H264ParserPrivate::getLevel(h264_stream_t *s)
{
    float level = 0.0f;
    if(s->nal->nal_unit_type != NALU_TYPE_SPS){
        return level;
    }
    level = s->sps->level_idc/10;
    return level;
}

string H264ParserPrivate::getSyntax(h264_stream_t *s)
{
    string syntax;
    const char* nal_unit_type_name;

    PRINT_TO_SYNTAX(syntax, " forbidden_zero_bit : %d \n", s->nal->forbidden_zero_bit );
    PRINT_TO_SYNTAX(syntax, " nal_ref_idc : %d \n", s->nal->nal_ref_idc );
    switch (s->nal->nal_unit_type)
    {
        case  NAL_UNIT_TYPE_UNSPECIFIED :                   nal_unit_type_name = "Unspecified"; break;
        case  NAL_UNIT_TYPE_CODED_SLICE_NON_IDR :           nal_unit_type_name = "Coded slice of a non-IDR picture"; break;
        case  NAL_UNIT_TYPE_CODED_SLICE_DATA_PARTITION_A :  nal_unit_type_name = "Coded slice data partition A"; break;
        case  NAL_UNIT_TYPE_CODED_SLICE_DATA_PARTITION_B :  nal_unit_type_name = "Coded slice data partition B"; break;
        case  NAL_UNIT_TYPE_CODED_SLICE_DATA_PARTITION_C :  nal_unit_type_name = "Coded slice data partition C"; break;
        case  NAL_UNIT_TYPE_CODED_SLICE_IDR :               nal_unit_type_name = "Coded slice of an IDR picture"; break;
        case  NAL_UNIT_TYPE_SEI :                           nal_unit_type_name = "Supplemental enhancement information (SEI)"; break;
        case  NAL_UNIT_TYPE_SPS :                           nal_unit_type_name = "Sequence parameter set"; break;
        case  NAL_UNIT_TYPE_PPS :                           nal_unit_type_name = "Picture parameter set"; break;
        case  NAL_UNIT_TYPE_AUD :                           nal_unit_type_name = "Access unit delimiter"; break;
        case  NAL_UNIT_TYPE_END_OF_SEQUENCE :               nal_unit_type_name = "End of sequence"; break;
        case  NAL_UNIT_TYPE_END_OF_STREAM :                 nal_unit_type_name = "End of stream"; break;
        case  NAL_UNIT_TYPE_FILLER :                        nal_unit_type_name = "Filler data"; break;
        case  NAL_UNIT_TYPE_SPS_EXT :                       nal_unit_type_name = "Sequence parameter set extension"; break;
                                                            // 14..18    // Reserved
        case  NAL_UNIT_TYPE_CODED_SLICE_AUX :               nal_unit_type_name = "Coded slice of an auxiliary coded picture without partitioning"; break;
                                                            // 20..23    // Reserved
                                                            // 24..31    // Unspecified
        default :                                           nal_unit_type_name = "Unknown"; break;
    }
    PRINT_TO_SYNTAX(syntax, " nal_unit_type : %d ( %s ) \n", s->nal->nal_unit_type, nal_unit_type_name );
    if( s->nal->nal_unit_type == NAL_UNIT_TYPE_CODED_SLICE_NON_IDR) { syntax.append(getSliceHeaderSyntax(s->sh)); }
    else if( s->nal->nal_unit_type == NAL_UNIT_TYPE_CODED_SLICE_IDR) { syntax.append(getSliceHeaderSyntax(s->sh)); }
    else if( s->nal->nal_unit_type == NAL_UNIT_TYPE_SPS) { syntax.append(getSPSSyntax(s->sps)); }
    else if( s->nal->nal_unit_type == NAL_UNIT_TYPE_PPS) { syntax.append(getPPSSyntax(s->pps)); }
    else if( s->nal->nal_unit_type == NAL_UNIT_TYPE_AUD) { syntax.append(getAudSyntax(s->aud)); }
    else if( s->nal->nal_unit_type == NAL_UNIT_TYPE_SEI) { syntax.append(getSEISyntax( s )); }
    return syntax;
}

string H264ParserPrivate::getSPSSyntax(sps_t *sps)
{
    string syntax;
    PRINT_TO_SYNTAX(syntax, " profile_idc : %d \n", sps->profile_idc );
    PRINT_TO_SYNTAX(syntax, " constraint_set0_flag : %d \n", sps->constraint_set0_flag );
    PRINT_TO_SYNTAX(syntax, " constraint_set1_flag : %d \n", sps->constraint_set1_flag );
    PRINT_TO_SYNTAX(syntax, " constraint_set2_flag : %d \n", sps->constraint_set2_flag );
    PRINT_TO_SYNTAX(syntax, " constraint_set3_flag : %d \n", sps->constraint_set3_flag );
    PRINT_TO_SYNTAX(syntax, " constraint_set4_flag : %d \n", sps->constraint_set4_flag );
    PRINT_TO_SYNTAX(syntax, " constraint_set5_flag : %d \n", sps->constraint_set5_flag );
    PRINT_TO_SYNTAX(syntax, " reserved_zero_2bits : %d \n", sps->reserved_zero_2bits );
    PRINT_TO_SYNTAX(syntax, " level_idc : %d \n", sps->level_idc );
    PRINT_TO_SYNTAX(syntax, " seq_parameter_set_id : %d \n", sps->seq_parameter_set_id );
    PRINT_TO_SYNTAX(syntax, " chroma_format_idc : %d \n", sps->chroma_format_idc );
    PRINT_TO_SYNTAX(syntax, " residual_colour_transform_flag : %d \n", sps->residual_colour_transform_flag );
    PRINT_TO_SYNTAX(syntax, " bit_depth_luma_minus8 : %d \n", sps->bit_depth_luma_minus8 );
    PRINT_TO_SYNTAX(syntax, " bit_depth_chroma_minus8 : %d \n", sps->bit_depth_chroma_minus8 );
    PRINT_TO_SYNTAX(syntax, " qpprime_y_zero_transform_bypass_flag : %d \n", sps->qpprime_y_zero_transform_bypass_flag );
    PRINT_TO_SYNTAX(syntax, " seq_scaling_matrix_present_flag : %d \n", sps->seq_scaling_matrix_present_flag );
    //  int seq_scaling_list_present_flag[8];
    //  void* ScalingList4x4[6];
    //  int UseDefaultScalingMatrix4x4Flag[6];
    //  void* ScalingList8x8[2];
    //  int UseDefaultScalingMatrix8x8Flag[2];
    PRINT_TO_SYNTAX(syntax, " log2_max_frame_num_minus4 : %d \n", sps->log2_max_frame_num_minus4 );
    PRINT_TO_SYNTAX(syntax, " pic_order_cnt_type : %d \n", sps->pic_order_cnt_type );
    PRINT_TO_SYNTAX(syntax, "   log2_max_pic_order_cnt_lsb_minus4 : %d \n", sps->log2_max_pic_order_cnt_lsb_minus4 );
    PRINT_TO_SYNTAX(syntax, "   delta_pic_order_always_zero_flag : %d \n", sps->delta_pic_order_always_zero_flag );
    PRINT_TO_SYNTAX(syntax, "   offset_for_non_ref_pic : %d \n", sps->offset_for_non_ref_pic );
    PRINT_TO_SYNTAX(syntax, "   offset_for_top_to_bottom_field : %d \n", sps->offset_for_top_to_bottom_field );
    PRINT_TO_SYNTAX(syntax, "   num_ref_frames_in_pic_order_cnt_cycle : %d \n", sps->num_ref_frames_in_pic_order_cnt_cycle );
    //  int offset_for_ref_frame[256];
    PRINT_TO_SYNTAX(syntax, " num_ref_frames : %d \n", sps->num_ref_frames );
    PRINT_TO_SYNTAX(syntax, " gaps_in_frame_num_value_allowed_flag : %d \n", sps->gaps_in_frame_num_value_allowed_flag );
    PRINT_TO_SYNTAX(syntax, " pic_width_in_mbs_minus1 : %d \n", sps->pic_width_in_mbs_minus1 );
    PRINT_TO_SYNTAX(syntax, " pic_height_in_map_units_minus1 : %d \n", sps->pic_height_in_map_units_minus1 );
    PRINT_TO_SYNTAX(syntax, " frame_mbs_only_flag : %d \n", sps->frame_mbs_only_flag );
    PRINT_TO_SYNTAX(syntax, " mb_adaptive_frame_field_flag : %d \n", sps->mb_adaptive_frame_field_flag );
    PRINT_TO_SYNTAX(syntax, " direct_8x8_inference_flag : %d \n", sps->direct_8x8_inference_flag );
    PRINT_TO_SYNTAX(syntax, " frame_cropping_flag : %d \n", sps->frame_cropping_flag );
    PRINT_TO_SYNTAX(syntax, "   frame_crop_left_offset : %d \n", sps->frame_crop_left_offset );
    PRINT_TO_SYNTAX(syntax, "   frame_crop_right_offset : %d \n", sps->frame_crop_right_offset );
    PRINT_TO_SYNTAX(syntax, "   frame_crop_top_offset : %d \n", sps->frame_crop_top_offset );
    PRINT_TO_SYNTAX(syntax, "   frame_crop_bottom_offset : %d \n", sps->frame_crop_bottom_offset );
    PRINT_TO_SYNTAX(syntax, " vui_parameters_present_flag : %d \n", sps->vui_parameters_present_flag );

    PRINT_TO_SYNTAX(syntax, "=== VUI ===\n");
    PRINT_TO_SYNTAX(syntax, " aspect_ratio_info_present_flag : %d \n", sps->vui.aspect_ratio_info_present_flag );
    PRINT_TO_SYNTAX(syntax, "   aspect_ratio_idc : %d \n", sps->vui.aspect_ratio_idc );
    PRINT_TO_SYNTAX(syntax, "     sar_width : %d \n", sps->vui.sar_width );
    PRINT_TO_SYNTAX(syntax, "     sar_height : %d \n", sps->vui.sar_height );
    PRINT_TO_SYNTAX(syntax, " overscan_info_present_flag : %d \n", sps->vui.overscan_info_present_flag );
    PRINT_TO_SYNTAX(syntax, "   overscan_appropriate_flag : %d \n", sps->vui.overscan_appropriate_flag );
    PRINT_TO_SYNTAX(syntax, " video_signal_type_present_flag : %d \n", sps->vui.video_signal_type_present_flag );
    PRINT_TO_SYNTAX(syntax, "   video_format : %d \n", sps->vui.video_format );
    PRINT_TO_SYNTAX(syntax, "   video_full_range_flag : %d \n", sps->vui.video_full_range_flag );
    PRINT_TO_SYNTAX(syntax, "   colour_description_present_flag : %d \n", sps->vui.colour_description_present_flag );
    PRINT_TO_SYNTAX(syntax, "     colour_primaries : %d \n", sps->vui.colour_primaries );
    PRINT_TO_SYNTAX(syntax, "   transfer_characteristics : %d \n", sps->vui.transfer_characteristics );
    PRINT_TO_SYNTAX(syntax, "   matrix_coefficients : %d \n", sps->vui.matrix_coefficients );
    PRINT_TO_SYNTAX(syntax, " chroma_loc_info_present_flag : %d \n", sps->vui.chroma_loc_info_present_flag );
    PRINT_TO_SYNTAX(syntax, "   chroma_sample_loc_type_top_field : %d \n", sps->vui.chroma_sample_loc_type_top_field );
    PRINT_TO_SYNTAX(syntax, "   chroma_sample_loc_type_bottom_field : %d \n", sps->vui.chroma_sample_loc_type_bottom_field );
    PRINT_TO_SYNTAX(syntax, " timing_info_present_flag : %d \n", sps->vui.timing_info_present_flag );
    PRINT_TO_SYNTAX(syntax, "   num_units_in_tick : %d \n", sps->vui.num_units_in_tick );
    PRINT_TO_SYNTAX(syntax, "   time_scale : %d \n", sps->vui.time_scale );
    PRINT_TO_SYNTAX(syntax, "   fixed_frame_rate_flag : %d \n", sps->vui.fixed_frame_rate_flag );
    PRINT_TO_SYNTAX(syntax, " nal_hrd_parameters_present_flag : %d \n", sps->vui.nal_hrd_parameters_present_flag );
    PRINT_TO_SYNTAX(syntax, " vcl_hrd_parameters_present_flag : %d \n", sps->vui.vcl_hrd_parameters_present_flag );
    PRINT_TO_SYNTAX(syntax, "   low_delay_hrd_flag : %d \n", sps->vui.low_delay_hrd_flag );
    PRINT_TO_SYNTAX(syntax, " pic_struct_present_flag : %d \n", sps->vui.pic_struct_present_flag );
    PRINT_TO_SYNTAX(syntax, " bitstream_restriction_flag : %d \n", sps->vui.bitstream_restriction_flag );
    PRINT_TO_SYNTAX(syntax, "   motion_vectors_over_pic_boundaries_flag : %d \n", sps->vui.motion_vectors_over_pic_boundaries_flag );
    PRINT_TO_SYNTAX(syntax, "   max_bytes_per_pic_denom : %d \n", sps->vui.max_bytes_per_pic_denom );
    PRINT_TO_SYNTAX(syntax, "   max_bits_per_mb_denom : %d \n", sps->vui.max_bits_per_mb_denom );
    PRINT_TO_SYNTAX(syntax, "   log2_max_mv_length_horizontal : %d \n", sps->vui.log2_max_mv_length_horizontal );
    PRINT_TO_SYNTAX(syntax, "   log2_max_mv_length_vertical : %d \n", sps->vui.log2_max_mv_length_vertical );
    PRINT_TO_SYNTAX(syntax, "   num_reorder_frames : %d \n", sps->vui.num_reorder_frames );
    PRINT_TO_SYNTAX(syntax, "   max_dec_frame_buffering : %d \n", sps->vui.max_dec_frame_buffering );

    PRINT_TO_SYNTAX(syntax, "=== HRD ===\n");
    PRINT_TO_SYNTAX(syntax, " cpb_cnt_minus1 : %d \n", sps->hrd.cpb_cnt_minus1 );
    PRINT_TO_SYNTAX(syntax, " bit_rate_scale : %d \n", sps->hrd.bit_rate_scale );
    PRINT_TO_SYNTAX(syntax, " cpb_size_scale : %d \n", sps->hrd.cpb_size_scale );
    int SchedSelIdx;
    for( SchedSelIdx = 0; SchedSelIdx <= sps->hrd.cpb_cnt_minus1; SchedSelIdx++ )
    {
        PRINT_TO_SYNTAX(syntax, "   bit_rate_value_minus1[%d] : %d \n", SchedSelIdx, sps->hrd.bit_rate_value_minus1[SchedSelIdx] ); // up to cpb_cnt_minus1, which is <= 31
        PRINT_TO_SYNTAX(syntax, "   cpb_size_value_minus1[%d] : %d \n", SchedSelIdx, sps->hrd.cpb_size_value_minus1[SchedSelIdx] );
        PRINT_TO_SYNTAX(syntax, "   cbr_flag[%d] : %d \n", SchedSelIdx, sps->hrd.cbr_flag[SchedSelIdx] );
    }
    PRINT_TO_SYNTAX(syntax, " initial_cpb_removal_delay_length_minus1 : %d \n", sps->hrd.initial_cpb_removal_delay_length_minus1 );
    PRINT_TO_SYNTAX(syntax, " cpb_removal_delay_length_minus1 : %d \n", sps->hrd.cpb_removal_delay_length_minus1 );
    PRINT_TO_SYNTAX(syntax, " dpb_output_delay_length_minus1 : %d \n", sps->hrd.dpb_output_delay_length_minus1 );
    PRINT_TO_SYNTAX(syntax, " time_offset_length : %d \n", sps->hrd.time_offset_length );
    return syntax;
}

string H264ParserPrivate::getPPSSyntax(pps_t *pps)
{
    string syntax;
    PRINT_TO_SYNTAX(syntax, " pic_parameter_set_id : %d \n", pps->pic_parameter_set_id );
    PRINT_TO_SYNTAX(syntax, " seq_parameter_set_id : %d \n", pps->seq_parameter_set_id );
    PRINT_TO_SYNTAX(syntax, " entropy_coding_mode_flag : %d \n", pps->entropy_coding_mode_flag );
    PRINT_TO_SYNTAX(syntax, " pic_order_present_flag : %d \n", pps->pic_order_present_flag );
    PRINT_TO_SYNTAX(syntax, " num_slice_groups_minus1 : %d \n", pps->num_slice_groups_minus1 );
    PRINT_TO_SYNTAX(syntax, " slice_group_map_type : %d \n", pps->slice_group_map_type );
    //  int run_length_minus1[8]; // up to num_slice_groups_minus1, which is <= 7 in Baseline and Extended, 0 otheriwse
    //  int top_left[8];
    //  int bottom_right[8];
    //  int slice_group_change_direction_flag;
    //  int slice_group_change_rate_minus1;
    //  int pic_size_in_map_units_minus1;
    //  int slice_group_id[256]; // FIXME what size?
    PRINT_TO_SYNTAX(syntax, " num_ref_idx_l0_active_minus1 : %d \n", pps->num_ref_idx_l0_active_minus1 );
    PRINT_TO_SYNTAX(syntax, " num_ref_idx_l1_active_minus1 : %d \n", pps->num_ref_idx_l1_active_minus1 );
    PRINT_TO_SYNTAX(syntax, " weighted_pred_flag : %d \n", pps->weighted_pred_flag );
    PRINT_TO_SYNTAX(syntax, " weighted_bipred_idc : %d \n", pps->weighted_bipred_idc );
    PRINT_TO_SYNTAX(syntax, " pic_init_qp_minus26 : %d \n", pps->pic_init_qp_minus26 );
    PRINT_TO_SYNTAX(syntax, " pic_init_qs_minus26 : %d \n", pps->pic_init_qs_minus26 );
    PRINT_TO_SYNTAX(syntax, " chroma_qp_index_offset : %d \n", pps->chroma_qp_index_offset );
    PRINT_TO_SYNTAX(syntax, " deblocking_filter_control_present_flag : %d \n", pps->deblocking_filter_control_present_flag );
    PRINT_TO_SYNTAX(syntax, " constrained_intra_pred_flag : %d \n", pps->constrained_intra_pred_flag );
    PRINT_TO_SYNTAX(syntax, " redundant_pic_cnt_present_flag : %d \n", pps->redundant_pic_cnt_present_flag );
    PRINT_TO_SYNTAX(syntax, " transform_8x8_mode_flag : %d \n", pps->transform_8x8_mode_flag );
    PRINT_TO_SYNTAX(syntax, " pic_scaling_matrix_present_flag : %d \n", pps->pic_scaling_matrix_present_flag );
    //  int pic_scaling_list_present_flag[8];
    //  void* ScalingList4x4[6];
    //  int UseDefaultScalingMatrix4x4Flag[6];
    //  void* ScalingList8x8[2];
    //  int UseDefaultScalingMatrix8x8Flag[2];
    PRINT_TO_SYNTAX(syntax, " second_chroma_qp_index_offset : %d \n", pps->second_chroma_qp_index_offset );
    return syntax;
}

string H264ParserPrivate::getSliceHeaderSyntax(slice_header_t *sh)
{
    string syntax;
    PRINT_TO_SYNTAX(syntax, "======= Slice Header =======\n");
    PRINT_TO_SYNTAX(syntax, " first_mb_in_slice : %d \n", sh->first_mb_in_slice );
    const char* slice_type_name;
    switch(sh->slice_type)
    {
        case SH_SLICE_TYPE_P :       slice_type_name = "P slice"; break;
        case SH_SLICE_TYPE_B :       slice_type_name = "B slice"; break;
        case SH_SLICE_TYPE_I :       slice_type_name = "I slice"; break;
        case SH_SLICE_TYPE_SP :      slice_type_name = "SP slice"; break;
        case SH_SLICE_TYPE_SI :      slice_type_name = "SI slice"; break;
        case SH_SLICE_TYPE_P_ONLY :  slice_type_name = "P slice only"; break;
        case SH_SLICE_TYPE_B_ONLY :  slice_type_name = "B slice only"; break;
        case SH_SLICE_TYPE_I_ONLY :  slice_type_name = "I slice only"; break;
        case SH_SLICE_TYPE_SP_ONLY : slice_type_name = "SP slice only"; break;
        case SH_SLICE_TYPE_SI_ONLY : slice_type_name = "SI slice only"; break;
        default :                    slice_type_name = "Unknown"; break;
    }
    PRINT_TO_SYNTAX(syntax, " slice_type : %d ( %s ) \n", sh->slice_type, slice_type_name );

    PRINT_TO_SYNTAX(syntax, " pic_parameter_set_id : %d \n", sh->pic_parameter_set_id );
    PRINT_TO_SYNTAX(syntax, " frame_num : %d \n", sh->frame_num );
    PRINT_TO_SYNTAX(syntax, " field_pic_flag : %d \n", sh->field_pic_flag );
    PRINT_TO_SYNTAX(syntax, " bottom_field_flag : %d \n", sh->bottom_field_flag );
    PRINT_TO_SYNTAX(syntax, " idr_pic_id : %d \n", sh->idr_pic_id );
    PRINT_TO_SYNTAX(syntax, " pic_order_cnt_lsb : %d \n", sh->pic_order_cnt_lsb );
    PRINT_TO_SYNTAX(syntax, " delta_pic_order_cnt_bottom : %d \n", sh->delta_pic_order_cnt_bottom );
    // int delta_pic_order_cnt[ 2 ];
    PRINT_TO_SYNTAX(syntax, " redundant_pic_cnt : %d \n", sh->redundant_pic_cnt );
    PRINT_TO_SYNTAX(syntax, " direct_spatial_mv_pred_flag : %d \n", sh->direct_spatial_mv_pred_flag );
    PRINT_TO_SYNTAX(syntax, " num_ref_idx_active_override_flag : %d \n", sh->num_ref_idx_active_override_flag );
    PRINT_TO_SYNTAX(syntax, " num_ref_idx_l0_active_minus1 : %d \n", sh->num_ref_idx_l0_active_minus1 );
    PRINT_TO_SYNTAX(syntax, " num_ref_idx_l1_active_minus1 : %d \n", sh->num_ref_idx_l1_active_minus1 );
    PRINT_TO_SYNTAX(syntax, " cabac_init_idc : %d \n", sh->cabac_init_idc );
    PRINT_TO_SYNTAX(syntax, " slice_qp_delta : %d \n", sh->slice_qp_delta );
    PRINT_TO_SYNTAX(syntax, " sp_for_switch_flag : %d \n", sh->sp_for_switch_flag );
    PRINT_TO_SYNTAX(syntax, " slice_qs_delta : %d \n", sh->slice_qs_delta );
    PRINT_TO_SYNTAX(syntax, " disable_deblocking_filter_idc : %d \n", sh->disable_deblocking_filter_idc );
    PRINT_TO_SYNTAX(syntax, " slice_alpha_c0_offset_div2 : %d \n", sh->slice_alpha_c0_offset_div2 );
    PRINT_TO_SYNTAX(syntax, " slice_beta_offset_div2 : %d \n", sh->slice_beta_offset_div2 );
    PRINT_TO_SYNTAX(syntax, " slice_group_change_cycle : %d \n", sh->slice_group_change_cycle );

    PRINT_TO_SYNTAX(syntax, "=== Prediction Weight Table ===\n");
    PRINT_TO_SYNTAX(syntax, " luma_log2_weight_denom : %d \n", sh->pwt.luma_log2_weight_denom );
    PRINT_TO_SYNTAX(syntax, " chroma_log2_weight_denom : %d \n", sh->pwt.chroma_log2_weight_denom );
    //   printf(" luma_weight_l0_flag : %d \n", sh->pwt.luma_weight_l0_flag );
    // int luma_weight_l0[64];
    // int luma_offset_l0[64];
    //    printf(" chroma_weight_l0_flag : %d \n", sh->pwt.chroma_weight_l0_flag );
    // int chroma_weight_l0[64][2];
    // int chroma_offset_l0[64][2];
    //   printf(" luma_weight_l1_flag : %d \n", sh->pwt.luma_weight_l1_flag );
    // int luma_weight_l1[64];
    // int luma_offset_l1[64];
    //    printf(" chroma_weight_l1_flag : %d \n", sh->pwt.chroma_weight_l1_flag );
    // int chroma_weight_l1[64][2];
    // int chroma_offset_l1[64][2];

    PRINT_TO_SYNTAX(syntax, "=== Ref Pic List Reordering ===\n");
    PRINT_TO_SYNTAX(syntax, " ref_pic_list_reordering_flag_l0 : %d \n", sh->rplr.ref_pic_list_reordering_flag_l0 );
    PRINT_TO_SYNTAX(syntax, " ref_pic_list_reordering_flag_l1 : %d \n", sh->rplr.ref_pic_list_reordering_flag_l1 );
    // int reordering_of_pic_nums_idc;
    // int abs_diff_pic_num_minus1;
    // int long_term_pic_num;

    PRINT_TO_SYNTAX(syntax, "=== Decoded Ref Pic Marking ===\n");
    PRINT_TO_SYNTAX(syntax, " no_output_of_prior_pics_flag : %d \n", sh->drpm.no_output_of_prior_pics_flag );
    PRINT_TO_SYNTAX(syntax, " long_term_reference_flag : %d \n", sh->drpm.long_term_reference_flag );
    PRINT_TO_SYNTAX(syntax, " adaptive_ref_pic_marking_mode_flag : %d \n", sh->drpm.adaptive_ref_pic_marking_mode_flag );
    // int memory_management_control_operation;
    // int difference_of_pic_nums_minus1;
    // int long_term_pic_num;
    // int long_term_frame_idx;
    // int max_long_term_frame_idx_plus1;
    return syntax;
}

string H264ParserPrivate::getAudSyntax(aud_t* aud)
{
    string syntax;
    PRINT_TO_SYNTAX(syntax, "======= Access Unit Delimiter =======\n");
    const char* primary_pic_type_name;
    switch (aud->primary_pic_type)
    {
        case AUD_PRIMARY_PIC_TYPE_I :       primary_pic_type_name = "I"; break;
        case AUD_PRIMARY_PIC_TYPE_IP :      primary_pic_type_name = "I, P"; break;
        case AUD_PRIMARY_PIC_TYPE_IPB :     primary_pic_type_name = "I, P, B"; break;
        case AUD_PRIMARY_PIC_TYPE_SI :      primary_pic_type_name = "SI"; break;
        case AUD_PRIMARY_PIC_TYPE_SISP :    primary_pic_type_name = "SI, SP"; break;
        case AUD_PRIMARY_PIC_TYPE_ISI :     primary_pic_type_name = "I, SI"; break;
        case AUD_PRIMARY_PIC_TYPE_ISIPSP :  primary_pic_type_name = "I, SI, P, SP"; break;
        case AUD_PRIMARY_PIC_TYPE_ISIPSPB : primary_pic_type_name = "I, SI, P, SP, B"; break;
        default : primary_pic_type_name = "Unknown"; break;
    }
    PRINT_TO_SYNTAX(syntax, " primary_pic_type : %d ( %s ) \n", aud->primary_pic_type, primary_pic_type_name );
    return syntax;
}

string H264ParserPrivate::getSEISyntax( h264_stream_t* h)
{
    string syntax;
    sei_t** seis = h->seis;
    int num_seis = h->num_seis;

    const char* sei_type_name;
    int i;
    for (i = 0; i < num_seis; i++)
    {
        sei_t* s = seis[i];
        switch(s->payloadType)
        {
            case SEI_TYPE_BUFFERING_PERIOD :          sei_type_name = "Buffering period"; break;
            case SEI_TYPE_PIC_TIMING :                sei_type_name = "Pic timing"; break;
            case SEI_TYPE_PAN_SCAN_RECT :             sei_type_name = "Pan scan rect"; break;
            case SEI_TYPE_FILLER_PAYLOAD :            sei_type_name = "Filler payload"; break;
            case SEI_TYPE_USER_DATA_REGISTERED_ITU_T_T35 : sei_type_name = "User data registered ITU-T T35"; break;
            case SEI_TYPE_USER_DATA_UNREGISTERED :    sei_type_name = "User data unregistered"; break;
            case SEI_TYPE_RECOVERY_POINT :            sei_type_name = "Recovery point"; break;
            case SEI_TYPE_DEC_REF_PIC_MARKING_REPETITION : sei_type_name = "Dec ref pic marking repetition"; break;
            case SEI_TYPE_SPARE_PIC :                 sei_type_name = "Spare pic"; break;
            case SEI_TYPE_SCENE_INFO :                sei_type_name = "Scene info"; break;
            case SEI_TYPE_SUB_SEQ_INFO :              sei_type_name = "Sub seq info"; break;
            case SEI_TYPE_SUB_SEQ_LAYER_CHARACTERISTICS : sei_type_name = "Sub seq layer characteristics"; break;
            case SEI_TYPE_SUB_SEQ_CHARACTERISTICS :   sei_type_name = "Sub seq characteristics"; break;
            case SEI_TYPE_FULL_FRAME_FREEZE :         sei_type_name = "Full frame freeze"; break;
            case SEI_TYPE_FULL_FRAME_FREEZE_RELEASE : sei_type_name = "Full frame freeze release"; break;
            case SEI_TYPE_FULL_FRAME_SNAPSHOT :       sei_type_name = "Full frame snapshot"; break;
            case SEI_TYPE_PROGRESSIVE_REFINEMENT_SEGMENT_START : sei_type_name = "Progressive refinement segment start"; break;
            case SEI_TYPE_PROGRESSIVE_REFINEMENT_SEGMENT_END : sei_type_name = "Progressive refinement segment end"; break;
            case SEI_TYPE_MOTION_CONSTRAINED_SLICE_GROUP_SET : sei_type_name = "Motion constrained slice group set"; break;
            case SEI_TYPE_FILM_GRAIN_CHARACTERISTICS : sei_type_name = "Film grain characteristics"; break;
            case SEI_TYPE_DEBLOCKING_FILTER_DISPLAY_PREFERENCE : sei_type_name = "Deblocking filter display preference"; break;
            case SEI_TYPE_STEREO_VIDEO_INFO :         sei_type_name = "Stereo video info"; break;
            default: sei_type_name = "Unknown"; break;
        }
        PRINT_TO_SYNTAX(syntax, "=== %s ===\n", sei_type_name);
        PRINT_TO_SYNTAX(syntax, " payloadType : %d \n", s->payloadType );
        PRINT_TO_SYNTAX(syntax, " payloadSize : %d \n", s->payloadSize );
        PRINT_TO_SYNTAX(syntax, " payload : ");
        syntax.append(hexToString(s->payload, s->payloadSize));
    }
    return syntax;
}

string H264ParserPrivate::hexToString(const uint8_t *buf, size_t size, int newline )
{
    string str;
    for(int i = 0; i < size; ++i) {
        char byte[16] = {0};
        sprintf(byte, "%02X ", buf[i]);
        str.append(byte);
        if(newline > 0 && (i+1) % newline == 0) {
            str.append("\n");
        }
    }
    return str;
}

bool H264ParserPrivate::close()
{
    if(m_isInit) {
        m_naluList.clear();
        m_isInit = false;
    }
    return true;
}

uint64_t H264ParserPrivate::getNaluCount()
{
    return m_naluList.size();
}

bool H264ParserPrivate::getNaluInfo(uint64_t idx, Nalu &nalu)
{
    if(idx >= m_naluList.size()) {
        return false;
    }
    nalu = m_naluList.at(idx);
    return true;
}

string H264ParserPrivate::getNaluSyntax(uint64_t idx)
{
    Nalu n;
    if(!m_isInit || !getNaluInfo(idx, n)) {
        return "";
    }
    string syntax;
    fstream stream(m_filepath, fstream::in | fstream::binary);
    if(!stream.is_open()) {
        LOGE(this, "Failed to open [%s]\n", m_filepath.c_str());
        return syntax;
    }
    const int32_t bufSize = n.length();
    uint8_t * buf = new (std::nothrow) uint8_t[bufSize];
    if(nullptr == buf) {
        LOGE(this, "Failed to apply for buffer[size:%d]!!\n", bufSize);
        return syntax;
    }
    stream.seekg(n.offset(), ios_base::beg);
    if(stream.read((char*)buf, bufSize).gcount() != bufSize) {
        stream.close();
        delete[] buf;
        return syntax;
    }

    uint8_t * p = buf;
    p += n.startCodeLength();
    h264_stream_t *s = h264_new();
    if(read_nal_unit(s, p, bufSize - n.startCodeLength()) <= 0) {
        stream.close();
        delete[] buf;
        h264_free(s);
        return syntax;
    }
    syntax = getSyntax(s);

    h264_free(s);
    delete[] buf;
    stream.close();

    return syntax;
}

bool H264ParserPrivate::getStreamInfo(H264StreamInfo &info)
{
    if(!m_isInit) {
        return false;
    }
    info = m_streamInfo;
    return true;
}

void H264ParserPrivate::showNalus()
{
    auto itr = m_naluList.begin();
    for(; itr != m_naluList.end(); ++itr) {
        itr->debugNalu();;
    }
}

void H264ParserPrivate::showStreamInfo()
{
    m_streamInfo.debugStreamInfo();
}
} // namespace jkit
