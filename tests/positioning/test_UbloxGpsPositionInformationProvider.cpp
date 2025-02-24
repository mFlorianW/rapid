// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <catch2/catch_all.hpp>
#include <catch2/trompeloeil.hpp>
#include <cc_ublox/Message.h>
#include <cc_ublox/frame/UbloxFrame.h>
#include <common/GpsPositionData.hpp>
#include <positioning/UbloxGpsPositionInformationProvider.hpp>
#include <testhelper/SignalSpy.hpp>
#include <testhelper/UbloxDevice.hpp>
#include <vector>

using namespace Rapid::Positioning;
using namespace Rapid::TestHelper;
using namespace Rapid::Common;
using namespace trompeloeil;

struct checksum_result
{
    std::uint8_t ck_a{0};
    std::uint8_t ck_b{0};
};

namespace
{
constexpr std::uint8_t CFG_MSG_CLASS_ID{0x06};
constexpr std::uint8_t CFG_MSG_MSG_ID{0x01};
constexpr std::uint8_t CFG_RATE_CLASS_ID{0x06};
constexpr std::uint8_t CFG_RATE_MSG_ID{0x08};
constexpr std::uint8_t NAV_PVT_CLASS_ID{0x01};
constexpr std::uint8_t NAV_PVT_MSG_ID{0x07};
} // namespace

struct __attribute__((packed)) CfgRateResp
{
    std::uint16_t preamble = 0x62B5;
    std::uint8_t classId = CFG_RATE_CLASS_ID;
    std::uint8_t msgId = CFG_RATE_MSG_ID;
    std::uint16_t length = 0x0006;
    std::uint16_t measRate = 0x0200;
    std::uint16_t navRate = 0x0200;
    std::uint16_t timeRef = 0x0200;
    std::uint8_t ckA = 0x00;
    std::uint8_t ckB = 0x00;
};

struct __attribute__((packed)) NavPvtResp
{
    std::uint16_t preamble = 0x62B5;
    std::uint8_t classId = NAV_PVT_CLASS_ID;
    std::uint8_t msgId = NAV_PVT_MSG_ID;
    std::uint16_t length = 0x005c;
    std::uint32_t itow = 0;
    std::uint16_t year = 1970;
    std::uint8_t month = 1;
    std::uint8_t day = 1;
    std::uint8_t hour = 1;
    std::uint8_t min = 1;
    std::uint8_t second = 1;
    std::uint8_t validFlags = 0;
    std::uint32_t tAcc = 0;
    std::int32_t nano = 1000000;
    std::uint8_t fixType = 0;
    std::uint8_t flags = 0;
    std::uint8_t flags2 = 0;
    std::uint8_t numSv = 12;
    std::int32_t lon = 100000000;
    std::int32_t lat = 110000000;
    std::int32_t height = 0;
    std::int32_t hMSL = 0;
    std::uint32_t hAcc = 0;
    std::uint32_t vAcc = 0;
    std::int32_t velN = 0;
    std::int32_t velE = 0;
    std::int32_t velD = 0;
    std::int32_t gSpeed = 15;
    std::int32_t headMot = 0;
    std::uint32_t sAcc = 0;
    std::uint32_t headAcc = 0;
    std::uint16_t pDOP = 0;
    std::uint16_t flags3 = 0;
    std::uint32_t reserved1 = 0;
    std::int32_t headVeh = 0;
    std::int16_t magDec = 0;
    std::uint16_t magAcc = 0;
    std::uint8_t ckA = 0x00;
    std::uint8_t ckB = 0x00;
};

struct TestFixture
{
    std::unique_ptr<UbloxDeviceMock> ubloxDevicePtr = std::make_unique<UbloxDeviceMock>();
    UbloxDeviceMock* ubloxDevice = ubloxDevicePtr.get();
    std::vector<std::uint8_t> CFG_MSG_POLL{0xB5, 0x62, 0x06, 0x01, 0x02, 0x00, 0x01, 0x07, 0x11, 0x3A};
    std::vector<std::uint8_t> CFG_RATE_POLL{0xB5, 0x62, 0x06, 0x08, 0x00, 0x00, 0x0e, 0x30};

    std::vector<std::uint8_t> createAckResp(std::uint8_t classId, std::uint8_t msgId)
    {
        auto ackMsg = std::vector<std::uint8_t>{0xB5, 0x62, 0x05, 0x01, 0x02, 0x00, classId, msgId, 0x00, 0x00};
        auto checksum = calculateChecksum(ackMsg.cbegin() + 2, 6);
        ackMsg[8] = checksum.ck_a;
        ackMsg[9] = checksum.ck_b;
        return ackMsg;
    }

    std::vector<std::uint8_t> createNackResp(std::uint8_t classId, std::uint8_t msgId)
    {
        auto nackResp = std::vector<std::uint8_t>{0xB5, 0x62, 0x05, 0x00, 0x02, 0x00, classId, msgId, 0x00, 0x00};
        auto checksum = calculateChecksum(nackResp.cbegin() + 2, 6);
        nackResp[8] = checksum.ck_a;
        nackResp[9] = checksum.ck_b;
        return nackResp;
    }

    std::vector<std::uint8_t> createCfgMsgResp(std::uint8_t rate)
    {
        auto cfgMsg = std::vector<std::uint8_t>{0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0x01, 0x07, rate, 0x00, 0x00};
        auto checksum = calculateChecksum(cfgMsg.cbegin() + 2, 7);
        cfgMsg[9] = checksum.ck_a;
        cfgMsg[10] = checksum.ck_b;
        return cfgMsg;
    }

    std::vector<std::uint8_t> createCfgRateResp(std::uint16_t measRate, std::uint16_t navRate, std::uint16_t timeRef)
    {
        auto cfgRate = CfgRateResp{};
        cfgRate.measRate = measRate;
        cfgRate.navRate = navRate;
        cfgRate.timeRef = timeRef;
        auto rawMsg = std::vector<std::uint8_t>(sizeof(CfgRateResp));
        std::memcpy(rawMsg.data(), &cfgRate, sizeof(CfgRateResp));
        auto checksum = calculateChecksum(rawMsg.cbegin() + 2, 10);
        rawMsg[12] = checksum.ck_a;
        rawMsg[13] = checksum.ck_b;
        return rawMsg;
    }

    std::vector<std::uint8_t> createNavPvtResp()
    {
        auto navPvt = NavPvtResp{};
        auto rawMsg = std::vector<std::uint8_t>(sizeof(navPvt));
        std::memcpy(rawMsg.data(), &navPvt, sizeof(navPvt));
        auto checksum = calculateChecksum(rawMsg.cbegin() + 2, 96);
        rawMsg[98] = checksum.ck_a;
        rawMsg[99] = checksum.ck_b;
        return rawMsg;
    }

    GpsPositionData createGpsPositionData()
    {
        auto posData = PositionData{11, 10};
        auto date = Date{"01.01.1970"};
        auto time = Timestamp{"01:01:01.001"};
        auto velocity = VelocityData{0.015};
        return {posData, time, date, velocity};
    }

    template <typename read_iterator>
    checksum_result calculateChecksum(read_iterator const& checksum_begin, size_t checksum_length)
    {
        std::uint16_t ck_a = 0;
        std::uint16_t ck_b = 0;

        for (size_t i = 0; i < checksum_length; ++i) {
            ck_a = ck_a + checksum_begin[i];
            ck_b = ck_b + ck_a;
        }

        ck_a = ck_a & 0xFF;
        ck_b = ck_b & 0xFF;

        return checksum_result{
            .ck_a = static_cast<std::uint8_t>(ck_a),
            .ck_b = static_cast<std::uint8_t>(ck_b),
        };
    }
};

TEST_CASE_METHOD(TestFixture, "The UbloxGpsPositionInformationProvider shall enable NAV_PVT message", "[CFG-MSG]")
{
    ALLOW_CALL(*ubloxDevice, write(_)).LR_WITH(_1 == CFG_RATE_POLL);
    SECTION("Request NAV_PVT configuration on initialied device")
    {
        REQUIRE_CALL(*ubloxDevice, write(_)).LR_WITH(_1 == CFG_MSG_POLL);
        REQUIRE_CALL(*ubloxDevice, isReady()).RETURN(true);
        auto ubloxGps = UbloxGpsPositionInformationProvider{std::move(ubloxDevicePtr)};
        (void)ubloxGps;
    }

    SECTION("Request NAV_PVT configuration after device ready signal")
    {
        REQUIRE_CALL(*ubloxDevice, write(_)).LR_WITH(_1 == CFG_MSG_POLL);
        REQUIRE_CALL(*ubloxDevice, isReady()).RETURN(false);
        auto ubloxGps = UbloxGpsPositionInformationProvider{std::move(ubloxDevicePtr)};
        ubloxDevice->ready.emit();
        (void)ubloxGps;
    }

    SECTION("Enable NAV_PVT message if not enabled")
    {
        auto callCount = 0;
        auto wrongCfgConfiguration = createCfgMsgResp(5);
        auto ackResponse = createAckResp(CFG_MSG_CLASS_ID, CFG_MSG_MSG_ID);
        auto successResponse = createAckResp(CFG_MSG_CLASS_ID, CFG_MSG_MSG_ID);
        successResponse.insert(successResponse.end(), wrongCfgConfiguration.begin(), wrongCfgConfiguration.end());
        REQUIRE_CALL(*ubloxDevice, write(_))
            .LR_SIDE_EFFECT(++callCount)
            .LR_SIDE_EFFECT(ubloxDevice->dataReady.emit())
            .LR_WITH(_1 == CFG_MSG_POLL);
        REQUIRE_CALL(*ubloxDevice, write(_)).LR_WITH(_1 == createCfgMsgResp(1)).LR_SIDE_EFFECT(++callCount);
        REQUIRE_CALL(*ubloxDevice, read()).LR_WITH(callCount == 1).LR_RETURN(successResponse);
        REQUIRE_CALL(*ubloxDevice, read()).LR_WITH(callCount == 2).LR_RETURN(ackResponse);
        REQUIRE_CALL(*ubloxDevice, isReady()).RETURN(true);
        auto ubloxGps = UbloxGpsPositionInformationProvider{std::move(ubloxDevicePtr)};
        ubloxDevice->dataReady.emit();
        (void)ubloxGps;
    }

    SECTION("Don't enable NAV_PVT message when already enabled")
    {
        auto correctConfiguration = createCfgMsgResp(1);
        auto successResponse = createAckResp(CFG_MSG_CLASS_ID, CFG_MSG_MSG_ID);
        successResponse.insert(successResponse.end(), correctConfiguration.begin(), correctConfiguration.end());
        REQUIRE_CALL(*ubloxDevice, write(_)).LR_SIDE_EFFECT(ubloxDevice->dataReady.emit()).LR_WITH(_1 == CFG_MSG_POLL);
        REQUIRE_CALL(*ubloxDevice, read()).LR_RETURN(successResponse);
        REQUIRE_CALL(*ubloxDevice, isReady()).RETURN(true);
        auto ubloxGps = UbloxGpsPositionInformationProvider{std::move(ubloxDevicePtr)};
        (void)ubloxGps;
    }

    SECTION("Emit the error signal when NAV PVT poll request failed")
    {
        REQUIRE_CALL(*ubloxDevice, write(_)).LR_WITH(_1 == CFG_MSG_POLL);
        REQUIRE_CALL(*ubloxDevice, isReady()).RETURN(true);
        REQUIRE_CALL(*ubloxDevice, read()).LR_RETURN(createNackResp(CFG_MSG_CLASS_ID, CFG_MSG_MSG_ID));
        auto ubloxGps = UbloxGpsPositionInformationProvider{std::move(ubloxDevicePtr)};
        auto errorSpy = SignalSpy{ubloxGps.errorOccured};
        ubloxDevice->dataReady.emit();
        REQUIRE(errorSpy.getCount() == 1);
    }

    SECTION("Emit the error signal when NAV PVT configuration request failed")
    {
        auto callCount = std::uint8_t{0};
        auto wrongCfgConfiguration = createCfgMsgResp(5);
        auto successResponse = createAckResp(CFG_MSG_CLASS_ID, CFG_MSG_MSG_ID);
        successResponse.insert(successResponse.end(), wrongCfgConfiguration.begin(), wrongCfgConfiguration.end());
        REQUIRE_CALL(*ubloxDevice, write(_))
            .LR_SIDE_EFFECT(++callCount)
            .LR_SIDE_EFFECT(ubloxDevice->dataReady.emit())
            .LR_WITH(_1 == CFG_MSG_POLL);
        REQUIRE_CALL(*ubloxDevice, write(_)).LR_SIDE_EFFECT(++callCount).LR_WITH(_1 == createCfgMsgResp(1));
        REQUIRE_CALL(*ubloxDevice, read()).LR_WITH(callCount == 1).LR_RETURN(successResponse);
        REQUIRE_CALL(*ubloxDevice, read())
            .LR_WITH(callCount == 2)
            .LR_RETURN(createNackResp(CFG_MSG_CLASS_ID, CFG_MSG_MSG_ID));
        REQUIRE_CALL(*ubloxDevice, isReady()).RETURN(true);
        auto ubloxGps = UbloxGpsPositionInformationProvider{std::move(ubloxDevicePtr)};
        auto errorSpy = SignalSpy{ubloxGps.errorOccured};
        ubloxDevice->dataReady.emit();
        REQUIRE(errorSpy.getCount() == 1);
    }
}

TEST_CASE_METHOD(TestFixture,
                 "The UbloxGpsPositionInformationProvider shall configure the measurement rate of the NAV_PVT message",
                 "[CFG-RATE]")
{
    ALLOW_CALL(*ubloxDevice, write(_)).LR_WITH(_1 == CFG_MSG_POLL);
    SECTION("Request CFG-RATE on creation with initialized device")
    {
        REQUIRE_CALL(*ubloxDevice, isReady()).RETURN(true);
        REQUIRE_CALL(*ubloxDevice, write(_)).LR_WITH(_1 == CFG_RATE_POLL);
        auto ubloxGps = UbloxGpsPositionInformationProvider{std::move(ubloxDevicePtr)};
        (void)ubloxGps;
    }

    SECTION("Request CFG-RATE on creation with uninitialized device")
    {
        REQUIRE_CALL(*ubloxDevice, isReady()).RETURN(false);
        REQUIRE_CALL(*ubloxDevice, write(_)).LR_WITH(_1 == CFG_RATE_POLL);
        auto ubloxGps = UbloxGpsPositionInformationProvider{std::move(ubloxDevicePtr)};
        (void)ubloxGps;
        ubloxDevice->ready.emit();
    }

    SECTION("Configure CFG-RATE when wrong configured")
    {
        auto callCount = 0;
        auto wrongRateCfg = createCfgRateResp(250, 24, 3);
        auto successResponse = createAckResp(CFG_RATE_CLASS_ID, CFG_RATE_MSG_ID);
        successResponse.insert(successResponse.end(), wrongRateCfg.begin(), wrongRateCfg.end());
        // 1. must check for initialied device.
        REQUIRE_CALL(*ubloxDevice, isReady()).RETURN(true);
        // 2. Send poll request for cfg rate
        REQUIRE_CALL(*ubloxDevice, write(_))
            .LR_SIDE_EFFECT(++callCount)
            .LR_SIDE_EFFECT(ubloxDevice->dataReady.emit())
            .LR_WITH(_1 == CFG_RATE_POLL);
        // 3. Give the success response with missconfigured cfg rate
        REQUIRE_CALL(*ubloxDevice, read()).LR_WITH(callCount == 1).RETURN(successResponse);
        // 4. Write the correct cfg rate configuration to the device
        REQUIRE_CALL(*ubloxDevice, write(_)).LR_SIDE_EFFECT(++callCount).LR_WITH(_1 == createCfgRateResp(40, 1, 0));
        // 5. Send ack response
        REQUIRE_CALL(*ubloxDevice, read())
            .LR_WITH(callCount == 2)
            .LR_RETURN(createAckResp(CFG_RATE_CLASS_ID, CFG_RATE_MSG_ID));
        auto ubloxGps = UbloxGpsPositionInformationProvider{std::move(ubloxDevicePtr)};
        (void)ubloxGps;
        ubloxDevice->dataReady.emit();
    }

    SECTION("Don't configure CFG-RATE when correctly configured")
    {
        auto callCount = 0;
        auto correctCfgRate = createCfgRateResp(40, 1, 0);
        auto successResponse = createAckResp(CFG_RATE_CLASS_ID, CFG_RATE_MSG_ID);
        successResponse.insert(successResponse.end(), correctCfgRate.begin(), correctCfgRate.end());
        // 1. must check for initialied device.
        REQUIRE_CALL(*ubloxDevice, isReady()).RETURN(true);
        // 2. Send poll request for CFG-RATE
        REQUIRE_CALL(*ubloxDevice, write(_))
            .LR_SIDE_EFFECT(++callCount)
            .LR_SIDE_EFFECT(ubloxDevice->dataReady.emit())
            .LR_WITH(_1 == CFG_RATE_POLL);
        // 3. Give the success response with missconfigured cfg rate and no further write should happen
        REQUIRE_CALL(*ubloxDevice, read()).LR_WITH(callCount == 1).RETURN(successResponse);
        auto ubloxGps = UbloxGpsPositionInformationProvider{std::move(ubloxDevicePtr)};
        (void)ubloxGps;
    }

    SECTION("Emit error signal when CFG-RATE poll failed")
    {
        // 1. mark the device as ready
        REQUIRE_CALL(*ubloxDevice, isReady()).RETURN(true);
        // 2. Send poll request for CFG-RATE
        REQUIRE_CALL(*ubloxDevice, write(_)).LR_WITH(_1 == CFG_RATE_POLL);
        // 3. SIMULATE NACK response from device
        REQUIRE_CALL(*ubloxDevice, read()).LR_RETURN(createNackResp(CFG_RATE_CLASS_ID, CFG_RATE_MSG_ID));
        auto ubloxGps = UbloxGpsPositionInformationProvider{std::move(ubloxDevicePtr)};
        auto errorSpy = SignalSpy{ubloxGps.errorOccured};
        ubloxDevice->dataReady.emit();
        REQUIRE(errorSpy.getCount() == 1);
    }

    SECTION("Emit error signal when CFG-RATE configuration request failed")
    {
        auto callCount = std::uint8_t{0};
        auto wrongCfgConfiguration = createCfgRateResp(1, 2, 3);
        auto successResponse = createAckResp(CFG_RATE_CLASS_ID, CFG_RATE_MSG_ID);
        successResponse.insert(successResponse.end(), wrongCfgConfiguration.begin(), wrongCfgConfiguration.end());
        // 1. mark the device as ready
        REQUIRE_CALL(*ubloxDevice, isReady()).RETURN(true);
        // 2. Send poll request for CFG-RATE
        REQUIRE_CALL(*ubloxDevice, write(_))
            .LR_SIDE_EFFECT(++callCount)
            .LR_SIDE_EFFECT(ubloxDevice->dataReady.emit())
            .LR_WITH(_1 == CFG_RATE_POLL);
        // 3. SIMULATE NACK response from device
        REQUIRE_CALL(*ubloxDevice, read()).LR_RETURN(successResponse);
        // 4. Write request with correct CFG-RATE configuration to the device
        REQUIRE_CALL(*ubloxDevice, write(_)).LR_SIDE_EFFECT(++callCount).LR_WITH(_1 == createCfgRateResp(40, 1, 0));
        // 5. Simulate NACK response from the device
        REQUIRE_CALL(*ubloxDevice, read())
            .LR_WITH(callCount == 2)
            .LR_RETURN(createNackResp(CFG_RATE_CLASS_ID, CFG_RATE_MSG_ID));
        auto ubloxGps = UbloxGpsPositionInformationProvider{std::move(ubloxDevicePtr)};
        auto errorSpy = SignalSpy{ubloxGps.errorOccured};
        ubloxDevice->dataReady.emit();
        REQUIRE(errorSpy.getCount() == 1);
    }
}

TEST_CASE_METHOD(TestFixture, "Upate the GPS position when NAV-PVT response received", "[NAV-PVT]")
{
    // Calls that are executed on the device but not important for the test
    ALLOW_CALL(*ubloxDevice, isReady()).LR_RETURN(true);
    ALLOW_CALL(*ubloxDevice, write(_));
    SECTION("Update GPS postion on new NAV-PVT message")
    {
        //1. Simulate NAV-PVT payload
        REQUIRE_CALL(*ubloxDevice, read()).LR_RETURN(createNavPvtResp());

        auto ubloxGps = UbloxGpsPositionInformationProvider{std::move(ubloxDevicePtr)};
        auto posChangedSyp = SignalSpy{ubloxGps.gpsPosition.valueChanged()};
        ubloxDevice->dataReady.emit();

        auto expectedGpsPos = createGpsPositionData();
        CHECK(ubloxGps.gpsPosition.get().getPosition() == expectedGpsPos.getPosition());
        CHECK(ubloxGps.gpsPosition.get().getDate() == expectedGpsPos.getDate());
        CHECK(ubloxGps.gpsPosition.get().getTime() == expectedGpsPos.getTime());
        CHECK(ubloxGps.gpsPosition.get().getVelocity() == expectedGpsPos.getVelocity());
        REQUIRE(posChangedSyp.getCount() == 1);
    }

    SECTION("Update satellite on new NAV-PVT message")
    {
        //1. Simulate NAV-PVT payload
        REQUIRE_CALL(*ubloxDevice, read()).LR_RETURN(createNavPvtResp());

        auto ubloxGps = UbloxGpsPositionInformationProvider{std::move(ubloxDevicePtr)};
        auto satelliteChanged = SignalSpy{ubloxGps.numberOfSatellitesChanged};
        ubloxDevice->dataReady.emit();

        CHECK(ubloxGps.getNumbersOfStatelite() == 12);
        REQUIRE(satelliteChanged.getCount() == 1);
    }
}
