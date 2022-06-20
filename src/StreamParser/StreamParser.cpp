#include "StreamParser.h"
#include <QtConcurrent/QtConcurrent>
StreamParser::StreamParser(QObject *parent)
    : QObject{parent}
{

}

void StreamParser::openStream(const QString &url)
{
    if(!m_h264Parser.isNull()) {
        return;
    }
    m_h264Parser.reset(new jkit::H264Parser(url.toStdString()));
    QtConcurrent::run([this](){
        if(m_h264Parser->init()) {
            jkit::H264StreamInfo info;
            m_h264Parser->getStreamInfo(info);
            m_isOpen = true;
            emit openFinished(streamInfoToJsonObj(info));
            return true;
        }
        return false;
    });
}

bool StreamParser::startParseStream()
{
    if(!m_isOpen) {
        return false;
    }
    return true;
}

bool StreamParser::closeStream()
{
    if(!m_isOpen) {
        return false;
    }
    m_h264Parser->deinit();
    return true;
}

QJsonObject StreamParser::streamInfo()
{
    QJsonObject ret;
    if(!m_isOpen) {
        return ret;
    }
    jkit::H264StreamInfo info;
    if(!m_h264Parser->getStreamInfo(info)) {
        return ret;
    }
    ret = streamInfoToJsonObj(info);
    return ret;
}

qint32 StreamParser::naluCount()
{
    if(!m_isOpen) {
        return 0;
    }
    return m_h264Parser->getNaluCount();
}

QJsonObject StreamParser::getNaluInfo(qint32 idx)
{
    QJsonObject ret;
    if(!m_isOpen) {
        return ret;
    }
    jkit::Nalu n;
    if(!m_h264Parser->getNaluInfo(idx, n)) {
       qWarning("Failed to get idx[%d] of nalu info", idx);
       return ret;
    }
    ret = naluToJsonObj(n);
    return ret;
}

QString StreamParser::getNaluSyntax(qint32 idx)
{
    QString ret;
    if(!m_isOpen) {
        return ret;
    }

    ret = m_h264Parser->getNaluSyntax(idx).c_str();
    return ret;
}

QJsonObject StreamParser::streamInfoToJsonObj(const jkit::H264StreamInfo &info)
{
    QJsonObject obj;
    obj.insert("width", info.width());
    obj.insert("height", info.heigth());
    obj.insert("duration", info.duration());
    obj.insert("level", info.level());
    obj.insert("fram_count", info.frameCount());
    obj.insert("frame_rate", info.frameRate());
    obj.insert("profile", info.profile().c_str());
    obj.insert("chroma_format", info.chromaFormat().c_str());
    return obj;
}

QJsonObject StreamParser::naluToJsonObj(const jkit::Nalu &n)
{
    QJsonObject obj;
    obj.insert("frame_type", getFrameType(n));
    obj.insert("length", n.length());
    obj.insert("frame_num", n.frameNum());
    obj.insert("offset", QString("0x%1").arg(n.offset(), 8, 16, QLatin1Char('0')));
    obj.insert("slice_type", n.sliceType());
    obj.insert("start_code", n.startCodeLength());
    return obj;
}

QString StreamParser::getFrameType(const jkit::Nalu &n)
{
    QString ret;
    switch(n.naluType()) {
    case jkit::NALU_TYPE_UNSPECIFIED:
        ret = "UNSPECIFIED";
        break;
    case jkit::NALU_TYPE_CODED_SLICE_DATA_PARTITION_A:
        ret = "CODED_SLICE_DATA_PARTITION_A";
        break;
    case jkit::NALU_TYPE_CODED_SLICE_DATA_PARTITION_B:
        ret = "CODED_SLICE_DATA_PARTITION_B";
        break;
    case jkit::NALU_TYPE_CODED_SLICE_DATA_PARTITION_C:
        ret = "CODED_SLICE_DATA_PARTITION_C";
        break;
    case jkit::NALU_TYPE_CODED_SLICE_NON_IDR:
    case jkit::NALU_TYPE_CODED_SLICE_IDR:
    case jkit::NALU_TYPE_CODED_SLICE_AUX:
        ret = n.sliceTypeToStr(n.sliceType()).c_str();
        break;
    case jkit::NALU_TYPE_SEI:
        ret = "SEI";
        break;
    case jkit::NALU_TYPE_SPS:
        ret = "SPS";
        break;
    case jkit::NALU_TYPE_PPS:
        ret = "PPS";
        break;
    case jkit::NALU_TYPE_AUD:
        ret = "AUD";
        break;
    case jkit::NALU_TYPE_END_OF_SEQUENCE:
        ret = "END_OF_SEQUENCE";
        break;
    case jkit::NALU_TYPE_END_OF_STREAM:
        ret = "END_OF_STREAM";
        break;
    case jkit::NALU_TYPE_FILLER:
        ret = "FILLER";
        break;
    case jkit::NALU_TYPE_SPS_EXT:
        ret = "SPS_EXT";
        break;
    default:
        ret = "UNSPECIFIED";
        break;
    }
    return ret;
}
