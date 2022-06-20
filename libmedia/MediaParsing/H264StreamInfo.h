#ifndef H264STREAMINFO_H
#define H264STREAMINFO_H
#include "../common.h"

namespace jkit {
class H264StreamInfoPrivate;
class MEDIA_EXPORT H264StreamInfo
{
public:
    H264StreamInfo();
    ~H264StreamInfo();

    H264StreamInfo(const H264StreamInfo&other);
    H264StreamInfo& operator=(const H264StreamInfo&);

    H264StreamInfo(H264StreamInfo&&other);
    H264StreamInfo& operator=(H264StreamInfo&&);

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
    std::shared_ptr<H264StreamInfoPrivate> m_d;
};
} // namespace jkit
#endif // H264STREAMINFO_H
