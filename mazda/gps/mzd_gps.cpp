
#include <dbus/dbus.h>
#include <dbus-c++/dbus.h>
#include <memory>

#include "../dbus/generated_cmu.h"

#define LOGTAG "mazda-gps"

#include "hu_uti.h"

#include "mzd_gps.h"

#define SERVICE_BUS_ADDRESS "unix:path=/tmp/dbus_service_socket"

enum LDSControl
{
    LDS_READ_START = 0,
    LDS_READ_STOP = 1
};

class GPSLDSCLient : public com::jci::lds::data_proxy,
        public DBus::ObjectProxy
{
public:
    GPSLDSCLient(DBus::Connection &connection)
        : DBus::ObjectProxy(connection, "/com/jci/lds/data", "com.jci.lds.data")
    {
    }

    virtual void GPSDiagnostics(const uint8_t& dTCId, const uint8_t& dTCAction) override {}
    virtual void OneTimeDRDiagnostics(const std::string& dRUnitVersion, const int32_t& antennaStatus, const bool& gyroSelfTest, const bool& accelSelfTest, const bool& resetLearning, const bool& saveLearning) override {}
    virtual void PeriodicDRDiagnostics(const int32_t& dRUnitStatus, const int32_t& speedPulse, const bool& reverse, const int32_t& dRUnitMode, const int32_t& gyroStatus, const int32_t& accelStatus) override {}

};

class GPSLDSControl : public com::jci::lds::control_proxy,
        public DBus::ObjectProxy
{
public:
    GPSLDSControl(DBus::Connection &connection)
        : DBus::ObjectProxy(connection, "/com/jci/lds/control", "com.jci.lds.control")
    {
    }

    virtual void ReadStatus(const int32_t& commandReply, const int32_t& status) override;
};

static std::unique_ptr<GPSLDSCLient> gps_client;
static std::unique_ptr<GPSLDSControl> gps_control;
static int get_data_errors_in_a_row = 0;

void GPSLDSControl::ReadStatus(const int32_t& commandReply, const int32_t& status)
{
    //not sure what this does yet
    logw("Read status changed commandReply %i status %i\n", commandReply, status);
}


void mzd_gps2_start()
{
    if (gps_client != NULL)
        return;

    try
    {
        DBus::Connection gpservice_bus(SERVICE_BUS_ADDRESS, false);
        gpservice_bus.register_bus();
        gps_client.reset(new GPSLDSCLient(gpservice_bus));
        gps_control.reset(new GPSLDSControl(gpservice_bus));
    }
    catch(DBus::Error& error)
    {
        loge("DBUS: Failed to connect to SERVICE bus %s: %s", error.name(), error.message());
        gps_client.reset();
        gps_control.reset();
        return;
    }

    printf("GPS service connection established.\n");
}

bool mzd_gps2_get(GPSData& data)
{
    if (gps_client == NULL)
        return false;

    try
    {
        gps_client->GetPosition(data.positionAccuracy, data.uTCtime, data.latitude, data.longitude, data.altitude, data.heading, data.velocity, data.horizontalAccuracy, data.verticalAccuracy);

        if (get_data_errors_in_a_row > 0)
        {
            loge("DBUS: GetPosition hid %i failures", get_data_errors_in_a_row);
            get_data_errors_in_a_row = 0;
        }

        //timestamp 0 means "invalid" and positionAccuracy 0 means "no lock"
        if (data.uTCtime == 0 || data.positionAccuracy == 0)
            return false;

        return true;
    }
    catch(DBus::Error& error)
    {
        get_data_errors_in_a_row++;
        //prevent insane log spam
        if (get_data_errors_in_a_row < 10)
        {
            loge("DBUS: GetPosition failed %s: %s", error.name(), error.message());
        }
        return false;
    }
}

void mzd_gps2_set_enabled(bool bEnabled)
{
    if (gps_control)
    {
        try
        {
            gps_control->ReadControl(bEnabled ? LDS_READ_START : LDS_READ_STOP);
        }
        catch(DBus::Error& error)
        {
            loge("DBUS: ReadControl failed %s: %s", error.name(), error.message());
        }
    }
}

void mzd_gps2_stop()
{
    gps_client.reset();
    gps_control.reset();
}

bool GPSData::IsSame(const GPSData& other) const
{
    if (uTCtime == 0 && other.uTCtime == 0)
        return true; //other members don't matter since timestamp 0 means "invalid"
    return positionAccuracy == other.positionAccuracy &&
            uTCtime == other.uTCtime &&
            int32_t(latitude * 1E7) == int32_t(other.latitude * 1E7) &&
            int32_t(longitude * 1E7) == int32_t(other.longitude * 1E7) &&
            altitude == other.altitude &&
            int32_t(heading * 1E7) == int32_t(other.heading * 1E7) &&
            int32_t(velocity * 1E7) == int32_t(other.velocity * 1E7) &&
            int32_t(horizontalAccuracy * 1E7) == int32_t(other.horizontalAccuracy * 1E7) &&
            int32_t(verticalAccuracy * 1E7) == int32_t(other.verticalAccuracy * 1E7);
}

