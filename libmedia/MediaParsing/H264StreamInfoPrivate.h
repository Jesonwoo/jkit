#ifndef H264STREAMINFOPRIVATE_H
#define H264STREAMINFOPRIVATE_H
#include "../common.h"

namespace jkit {
class H264StreamInfoPrivate
{
public:
    H264StreamInfoPrivate();
    ~H264StreamInfoPrivate();


    H264StreamInfoPrivate(const H264StreamInfoPrivate&other);
    H264StreamInfoPrivate& operator=(const H264StreamInfoPrivate &other);

    H264StreamInfoPrivate(H264StreamInfoPrivate&&other);
    H264StreamInfoPrivate &operator=(H264StreamInfoPrivate &&other);



    int32_t width() const;
    void setWidth(int32_t newWidth);

    int32_t heigth() const;
    void setHeigth(int32_t newHeigth);

    int32_t duration() const;
    void setDuration(int32_t newDuration);

    int64_t frameCount() const;
    void setFrameCount(int64_t newFrameCount);

    float level() const;
    void setLevel(float newLevel);

    float frameRate() const;
    void setFrameRate(float newFrameRate);

    const std::string &profile() const;
    void setProfile(const std::string &newProfile);

    const std::string &chromaFormat() const;
    void setChromaFormat(const std::string &newChromaFormat);

    // TEST
    void debugStreamInfo();


private:
    int32_t m_width = 0;
    int32_t m_heigth = 0;
    int32_t m_duration = 0;       // 最大时长
    int64_t m_frameCount = 0;     // 帧数量
    float m_level = 0;            // 码流级别
    float m_frameRate = 0.0f;     // 最大帧率
/*
1、Baseline Profile：基本画质。支持I/P 帧，只支持无交错（Progressive）和CAVLC；
2、Extended profile：进阶画质。支持I/P/B/SP/SI 帧，只支持无交错（Progressive）和CAVLC；(用的少)
3、Main profile：主流画质。提供I/P/B 帧，支持无交错（Progressive）和交错（Interlaced）， 也支持CAVLC 和CABAC 的支持；
4、High profile：高级画质。在main Profile 的基础上增加了8x8内部预测、自定义量化、 无损视频编码和更多的YUV 格式；
   H.264 Baseline profile、Extended profile和Main profile都是针对8位样本数据、4:2:0格式(YUV)的视频序列。
   在相同配置情况下，High profile（HP）可以比Main profile（MP）降低10%的码率。 根据应用领域的不同，
   Baseline profile多应用于实时通信领域，Main profile多应用于流媒体领域，High profile则多应用于广电和存储领域。
*/
    std::string m_profile;      // 画质级别
    std::string m_chromaFormat; // 色度采样结构
};
} // namespace jkit
#endif // H264STREAMINFOPRIVATE_H
