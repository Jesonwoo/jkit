#include "H264StreamInfo.h"
#include "H264StreamInfoPrivate.h"
namespace jkit {
H264StreamInfo::H264StreamInfo()
    : m_d(new H264StreamInfoPrivate)
{

}

H264StreamInfo::~H264StreamInfo()
{
}

H264StreamInfo::H264StreamInfo(const H264StreamInfo &other)
    :H264StreamInfo()
{
    *m_d = *other.m_d;
}

H264StreamInfo &H264StreamInfo::operator =(const H264StreamInfo &other)
{
    if(this != &other) {
        *m_d = *other.m_d;
    }
    return *this;
}

H264StreamInfo::H264StreamInfo(H264StreamInfo &&other)
    :H264StreamInfo()
{
    *m_d = std::move(*other.m_d);
}

H264StreamInfo &H264StreamInfo::operator=(H264StreamInfo &&other)
{
    if(this != &other) {
        *m_d = std::move(*other.m_d);
    }
    return *this;
}
const std::string &H264StreamInfo::chromaFormat() const
{
    return m_d->chromaFormat();
}

void H264StreamInfo::setChromaFormat(const std::string &newChromaFormat)
{
    m_d->setChromaFormat(newChromaFormat);
}

const std::string &H264StreamInfo::profile() const
{
    return m_d->profile();
}

void H264StreamInfo::setProfile(const std::string &newProfile)
{
    m_d->setProfile(newProfile);
}

float H264StreamInfo::frameRate() const
{
    return m_d->frameRate();
}

void H264StreamInfo::setFrameRate(float newFrameRate)
{
    m_d->setFrameRate(newFrameRate);
}

float H264StreamInfo::level() const
{
    return m_d->level();
}

void H264StreamInfo::setLevel(float newLevel)
{
    m_d->setLevel(newLevel);
}

int64_t H264StreamInfo::frameCount() const
{
    return m_d->frameCount();
}

void H264StreamInfo::setFrameCount(int64_t newFrameCount)
{
    m_d->setFrameCount(newFrameCount);
}

int32_t H264StreamInfo::duration() const
{
    return m_d->duration();
}

void H264StreamInfo::setDuration(int32_t newDuration)
{
    m_d->setDuration(newDuration);
}

int32_t H264StreamInfo::heigth() const
{
    return m_d->heigth();
}

void H264StreamInfo::setHeigth(int32_t newHeigth)
{
    m_d->setHeigth(newHeigth);
}

int32_t H264StreamInfo::width() const
{
    return m_d->width();
}

void H264StreamInfo::setWidth(int32_t newWidth)
{
    m_d->setWidth(newWidth);
}

void H264StreamInfo::debugStreamInfo()
{
    m_d->debugStreamInfo();
}

} // namespace jkit
