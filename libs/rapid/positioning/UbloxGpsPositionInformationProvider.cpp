// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "UbloxGpsPositionInformationProvider.hpp"
#include <cc_ublox/Message.h>
#include <cc_ublox/frame/UbloxFrame.h>
#include <comms/process.h>
#include <comms/units.h>
#include <spdlog/spdlog.h>

namespace Rapid::Positioning
{
class UbloxGpsPositionInformationProviderPrivate
{
    using InMessage = cc_ublox::Message<comms::option::ReadIterator<std::uint8_t*>,
                                        comms::option::Handler<UbloxGpsPositionInformationProviderPrivate>>;

    using Outbuffer = std::vector<std::uint8_t>;
    using OutMessage = cc_ublox::Message<comms::option::IdInfoInterface,
                                         comms::option::WriteIterator<std::back_insert_iterator<Outbuffer>>,
                                         comms::option::LengthInfoInterface>;

    using InAckMsg = cc_ublox::message::AckAck<InMessage>;
    using InNakMsg = cc_ublox::message::AckNak<InMessage>;
    using InCfgMsgCurrent = cc_ublox::message::CfgMsgCurrent<InMessage>;
    using InCfgRateMsg = cc_ublox::message::CfgRate<InMessage>;
    using InNavPvtMsg = cc_ublox::message::NavPvt<InMessage>;

    using AllInMessages = std::tuple<InNavPvtMsg, InNakMsg, InAckMsg, InCfgMsgCurrent, InCfgRateMsg>;
    using UbloxFrame = cc_ublox::frame::UbloxFrame<InMessage, AllInMessages>;

public:
    UbloxGpsPositionInformationProviderPrivate(UbloxGpsPositionInformationProvider* q,
                                               std::unique_ptr<IUbloxDevice> device)
        : ubloxDevice{std::move(device)}
        , mQ{q}
    {
        std::ignore = ubloxDevice->dataReady.connect([this] {
            processData();
        });

        if (ubloxDevice->isReady()) {
            pollCfgMsg();
            pollCfgRate();
        } else {
            std::ignore = ubloxDevice->ready.connect([this] {
                pollCfgMsg();
                pollCfgRate();
            });
        }
    }

    void handle(InMessage const& msg)
    {
        SPDLOG_INFO("Received not requested message");
    }

    void handle(InAckMsg const& ack)
    {
        auto const msgId = ack.field_msgId().getValue();
        if (not mCfgMsgPolled and msgId == cc_ublox::MsgId::MsgId_CfgMsg) {
            SPDLOG_INFO("Message configuration for NAV-PVT successful requested");
            mCfgMsgPolled = true;
        } else if (msgId == cc_ublox::MsgId::MsgId_CfgMsg) {
            SPDLOG_INFO("CFG-MSG configuration for NAV-PVT  successful configured");
        } else if (not mCfgRatePolled and msgId == cc_ublox::MsgId::MsgId_CfgRate) {
            SPDLOG_INFO("CFG-Rate configuration successful polled");
            mCfgRatePolled = true;
        } else if (mCfgRatePolled and msgId == cc_ublox::MsgId::MsgId_CfgRate) {
            SPDLOG_INFO("CFG-Rate configuration successful configured");
        }
    }

    void handle(InNakMsg const& nack)
    {
        auto const msgId = nack.field_msgId().getValue();
        if (not mCfgMsgPolled and msgId == cc_ublox::MsgId::MsgId_CfgMsg) {
            mQ->errorOccured.emit("Failed to poll the NAV-PVT message configuration");
        } else if (mCfgMsgPolled and msgId == cc_ublox::MsgId::MsgId_CfgMsg) {
            mQ->errorOccured.emit("Failed to enable NAV-PVT message");
        } else if (not mCfgRatePolled and msgId == cc_ublox::MsgId::MsgId_CfgRate) {
            mQ->errorOccured.emit("Failed to poll the CFG-RATE configuration");
        } else if (mCfgRatePolled and msgId == cc_ublox::MsgId::MsgId_CfgRate) {
            mQ->errorOccured.emit("Failed to configure the CFG-RATE configuration");
        }
    }

    void handle(InCfgMsgCurrent const& msg)
    {
        if (msg.field_rate().getValue() != 1) {
            using OutCfgMsg = cc_ublox::message::CfgMsgCurrent<OutMessage>;
            auto outMsg = OutCfgMsg{};
            outMsg.field_msgId().setValue(cc_ublox::MsgId_NavPvt);
            outMsg.field_rate().setValue(1);
            auto rawMsg = serialize(outMsg);
            ubloxDevice->write(rawMsg);
            SPDLOG_INFO("Send NAV PVT message with rate set to 1");
        } else {
            SPDLOG_INFO("NAV-PVT message already correctly configured");
        }
    }

    void handle(InCfgRateMsg const& cfgRate)
    {
        using TimeRefEnum = cc_ublox::message::CfgRateFieldsCommon::TimeRefCommon::ValueType;
        auto measRate = cfgRate.field_measRate().getValue();
        auto navRate = cfgRate.field_navRate().getValue();
        auto timeRef = cfgRate.field_timeRef().value();
        if (measRate != 100 or navRate != 1 or timeRef != TimeRefEnum::UTC) {
            using OutCfgRateMsg = cc_ublox::message::CfgRate<OutMessage>;
            auto outMsg = OutCfgRateMsg{};
            outMsg.field_measRate().setValue(100);
            outMsg.field_navRate().setValue(1);
            outMsg.field_timeRef().setValue(TimeRefEnum::UTC);
            auto rawMsg = serialize(outMsg);
            ubloxDevice->write(rawMsg);
            SPDLOG_INFO("Send CFG RATE configuration meas rate {}, nav rate {}, time ref {}",
                        outMsg.field_measRate().value(),
                        outMsg.field_navRate().value(),
                        static_cast<int>(cfgRate.field_timeRef().value()));
        } else {
            SPDLOG_INFO("CFG-Rate already correctly configured");
        }
    }

    void handle(InNavPvtMsg const& navPvt)
    {
        auto const lat = comms::units::getDegrees<float>(navPvt.field_lat());
        auto const lon = comms::units::getDegrees<float>(navPvt.field_lon());
        auto pos = Common::PositionData{lat, lon};

        auto year = navPvt.field_year().getValue();
        auto day = comms::units::getDays<int>(navPvt.field_day());
        auto month = navPvt.field_month().getValue();
        auto date = Common::Date{};
        date.setYear(year);
        date.setMonth(month);
        date.setDay(day);

        auto const hour = comms::units::getHours<int>(navPvt.field_hour());
        auto const min = comms::units::getMinutes<int>(navPvt.field_min());
        auto const sec = comms::units::getSeconds<int>(navPvt.field_sec());
        int const fracMs = static_cast<int>(comms::units::getNanoseconds<int>(navPvt.field_nano()) * 1e-6);
        auto time = Common::Timestamp{};
        time.setHour(hour);
        time.setMinute(min);
        time.setSecond(sec);
        time.setFractionalOfSecond(fracMs);

        auto speedMeterPerSecond = comms::units::getMillimetersPerSecond<double>(navPvt.field_gSpeed()) / 1000;
        auto velocity = Common::VelocityData{speedMeterPerSecond};
        mQ->gpsPosition.set({pos, time, date, velocity});

        auto const sats = navPvt.field_numSV().getValue();
        if (sats != numberOfSatellites) {
            numberOfSatellites = sats;
            mQ->numberOfSatellitesChanged.emit();
        }
    }

    void pollCfgMsg()
    {
        using OutCfgMsgPoll = cc_ublox::message::CfgMsgPoll<OutMessage>;
        auto msg = OutCfgMsgPoll{};
        msg.field_msgId().setValue(cc_ublox::MsgId::MsgId_NavPvt);
        auto rawMsg = serialize(msg);
        ubloxDevice->write(rawMsg);
        SPDLOG_INFO("Send CFG-MSG poll request");
    }

    void pollCfgRate()
    {
        using OutCfgRatePoll = cc_ublox::message::CfgRatePoll<OutMessage>;
        auto msg = OutCfgRatePoll{};
        auto rawMsg = serialize(msg);
        ubloxDevice->write(rawMsg);
        SPDLOG_INFO("Send CFG-Rate poll request");
    }

    std::vector<uint8_t> serialize(OutMessage const& msg)
    {
        auto frame = UbloxFrame{};
        auto buffer = Outbuffer{};
        buffer.reserve(msg.length());
        auto iter = std::back_insert_iterator(buffer);
        auto es = frame.write(msg, iter, buffer.max_size());
        if (es == comms::ErrorStatus::UpdateRequired) {
            auto updateIter = &buffer[0];
            es = frame.update(updateIter, buffer.size());
        }

        if (es != comms::ErrorStatus::Success) {
            SPDLOG_ERROR("Failed to send ublox msg {} with error: {}",
                         static_cast<int>(msg.getId()),
                         static_cast<int>(es));
            return {};
        }
        return buffer;
    }

    void processData()
    {
        auto data = ubloxDevice->read();
        comms::processAllWithDispatch(data.data(), data.size(), frame, *this);
    }

    void navPvtSuccessfulConfigured()
    {
        SPDLOG_INFO("Message configuration for NAV-PVT successful configured");
    }

    void cfgRateSuccessfulConfigured()
    {
        SPDLOG_INFO("CFG-Rate configuration successful configured");
    }

    std::unique_ptr<IUbloxDevice> ubloxDevice = nullptr;
    UbloxFrame frame;
    bool mCfgMsgPolled = false;
    UbloxGpsPositionInformationProvider* mQ;
    bool mCfgRatePolled = false;
    std::uint8_t numberOfSatellites = 0;
};

UbloxGpsPositionInformationProvider::UbloxGpsPositionInformationProvider(std::unique_ptr<IUbloxDevice> dataProvider)
    : mD{std::make_unique<UbloxGpsPositionInformationProviderPrivate>(this, std::move(dataProvider))}
{
}

UbloxGpsPositionInformationProvider::~UbloxGpsPositionInformationProvider() = default;

std::uint8_t UbloxGpsPositionInformationProvider::getNumbersOfStatelite() const
{
    return mD->numberOfSatellites;
}

} // namespace Rapid::Positioning
