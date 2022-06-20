#include "H264Parser.h"
#include "H264ParserPrivate.h"
namespace jkit{
H264Parser::H264Parser(const std::string &filepath)
    : m_d(new H264ParserPrivate(filepath))
{
}

bool H264Parser::init()
{
    return m_d->init();
}

bool H264Parser::deinit()
{
    return m_d->deinit();
}

uint64_t H264Parser::getNaluCount()
{
    return m_d->getNaluCount();
}

bool H264Parser::getNaluInfo(uint64_t idx, Nalu &nalu)
{
    return m_d->getNaluInfo(idx, nalu);
}

bool H264Parser::getStreamInfo(H264StreamInfo &info)
{
    return m_d->getStreamInfo(info);
}

string H264Parser::getNaluSyntax(uint64_t idx)
{
    return m_d->getNaluSyntax(idx);
}

string H264Parser::filepath()
{
    return m_d->filepath();
}

void H264Parser::showNalus()
{
    return m_d->showNalus();
}

void H264Parser::showStreamInfo()
{
    return m_d->showStreamInfo();
}
} // namespace jkit
