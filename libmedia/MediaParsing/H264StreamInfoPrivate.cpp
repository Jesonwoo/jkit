#include "H264StreamInfoPrivate.h"
namespace jkit {
H264StreamInfoPrivate::H264StreamInfoPrivate()
{

}

int32_t H264StreamInfoPrivate::width() const
{
    return m_width;
}

void H264StreamInfoPrivate::setWidth(int32_t newWidth)
{
    m_width = newWidth;
}

int32_t H264StreamInfoPrivate::heigth() const
{
    return m_heigth;
}

void H264StreamInfoPrivate::setHeigth(int32_t newHeigth)
{
    m_heigth = newHeigth;
}

int32_t H264StreamInfoPrivate::duration() const
{
    return m_duration;
}

void H264StreamInfoPrivate::setDuration(int32_t newDuration)
{
    m_duration = newDuration;
}

int64_t H264StreamInfoPrivate::frameCount() const
{
    return m_frameCount;
}

void H264StreamInfoPrivate::setFrameCount(int64_t newFrameCount)
{
    m_frameCount = newFrameCount;
}

float H264StreamInfoPrivate::frameRate() const
{
    return m_frameRate;
}

void H264StreamInfoPrivate::setFrameRate(float newFrameRate)
{
    m_frameRate = newFrameRate;
}

float H264StreamInfoPrivate::level() const
{
    return m_level;
}

void H264StreamInfoPrivate::setLevel(float newLevel)
{
    m_level = newLevel;
}

const std::string &H264StreamInfoPrivate::profile() const
{
    return m_profile;
}

void H264StreamInfoPrivate::setProfile(const std::string &newProfile)
{
    m_profile = newProfile;
}

const std::string &H264StreamInfoPrivate::chromaFormat() const
{
    return m_chromaFormat;
}

void H264StreamInfoPrivate::setChromaFormat(const std::string &newChromaFormat)
{
    m_chromaFormat = newChromaFormat;
}

H264StreamInfoPrivate::~H264StreamInfoPrivate()
{

}

H264StreamInfoPrivate::H264StreamInfoPrivate(const H264StreamInfoPrivate &other)
{
    m_width         = other.m_width;
    m_heigth        = other.m_heigth;
    m_duration      = other.m_duration;
    m_frameCount    = other.m_frameCount;
    m_level         = other.m_level;
    m_frameRate     = other.m_frameRate;
    m_profile       = other.m_profile;
    m_chromaFormat  = other.m_chromaFormat;
}

H264StreamInfoPrivate &H264StreamInfoPrivate::operator=(const H264StreamInfoPrivate &other)
{
    if(this != &other) {
        m_width         = other.m_width;
        m_heigth        = other.m_heigth;
        m_duration       = other.m_duration;
        m_frameCount    = other.m_frameCount;
        m_level         = other.m_level;
        m_frameRate     = other.m_frameRate;
        m_profile       = other.m_profile;
        m_chromaFormat  = other.m_chromaFormat;
    }
    return *this;
}

H264StreamInfoPrivate &H264StreamInfoPrivate::operator=(H264StreamInfoPrivate &&other)
{
    if(this != &other) {
        m_width        = std::move(other.m_width);
        m_heigth       = std::move(other.m_heigth);
        m_duration     = std::move(other.m_duration);
        m_frameCount   = std::move(other.m_frameCount);
        m_level        = std::move(other.m_level);
        m_frameRate    = std::move(other.m_frameRate);
        m_profile      = std::move(other.m_profile);
        m_chromaFormat = std::move(other.m_chromaFormat);
    }
    return *this;
}

H264StreamInfoPrivate::H264StreamInfoPrivate(H264StreamInfoPrivate &&other)
{
    m_width         = std::move(other.m_width);
    m_heigth        = std::move(other.m_heigth);
    m_duration      = std::move(other.m_duration);
    m_frameCount    = std::move(other.m_frameCount);
    m_level         = std::move(other.m_level);
    m_frameRate     = std::move(other.m_frameRate);
    m_profile       = std::move(other.m_profile);
    m_chromaFormat  = std::move(other.m_chromaFormat);
}

void H264StreamInfoPrivate::debugStreamInfo()
{
    printf("==================== stream info ====================\n");
    printf("resolution:     %lux%lu\n", m_width, m_heigth);
    printf("duration:       %d\n", m_duration);
    printf("frameCount:     %d\n", m_frameCount);

    printf("level:          %f\n", m_level);
    printf("frameRate:      %f\n", m_frameRate);
    printf("chromaFormat:   %s\n", m_chromaFormat.c_str());
    printf("profile:        %s\n", m_profile.c_str());

    fflush(stdout);
}

} // namespace jkit
