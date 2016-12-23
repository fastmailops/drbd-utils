#include <DrbdVolume.h>
#include <DrbdConnection.h>
#include <utils.h>

const std::string DrbdVolume::PROP_KEY_VOL_NR      = "volume";
const std::string DrbdVolume::PROP_KEY_MINOR       = "minor";
const std::string DrbdVolume::PROP_KEY_DISK        = "disk";
const std::string DrbdVolume::PROP_KEY_PEER_DISK   = "peer-disk";
const std::string DrbdVolume::PROP_KEY_REPLICATION = "replication";

const char* DrbdVolume::DS_LABEL_DISKLESS     = "Diskless";
const char* DrbdVolume::DS_LABEL_ATTACHING    = "Attaching";
const char* DrbdVolume::DS_LABEL_DETACHING    = "Detaching";
const char* DrbdVolume::DS_LABEL_FAILED       = "Failed";
const char* DrbdVolume::DS_LABEL_NEGOTIATING  = "Negotiating";
const char* DrbdVolume::DS_LABEL_INCONSISTENT = "Inconsistent";
const char* DrbdVolume::DS_LABEL_OUTDATED     = "Outdated";
const char* DrbdVolume::DS_LABEL_UNKNOWN      = "DUnknown";
const char* DrbdVolume::DS_LABEL_CONSISTENT   = "Consistent";
const char* DrbdVolume::DS_LABEL_UP_TO_DATE   = "UpToDate";

const char* DrbdVolume::RS_LABEL_OFF                  = "Off";
const char* DrbdVolume::RS_LABEL_ESTABLISHED          = "Established";
const char* DrbdVolume::RS_LABEL_STARTING_SYNC_SOURCE = "StartingSyncS";
const char* DrbdVolume::RS_LABEL_STARTING_SYNC_TARGET = "StartingSyncT";
const char* DrbdVolume::RS_LABEL_WF_BITMAP_SOURCE     = "WFBitMapS";
const char* DrbdVolume::RS_LABEL_WF_BITMAP_TARGET     = "WFBitMapT";
const char* DrbdVolume::RS_LABEL_WF_SYNC_UUID         = "WFSyncUUID";
const char* DrbdVolume::RS_LABEL_SYNC_SOURCE          = "SyncSource";
const char* DrbdVolume::RS_LABEL_SYNC_TARGET          = "SyncTarget";
const char* DrbdVolume::RS_LABEL_PAUSED_SYNC_SOURCE   = "PausedSyncS";
const char* DrbdVolume::RS_LABEL_PAUSED_SYNC_TARGET   = "PausedSyncT";
const char* DrbdVolume::RS_LABEL_VERIFY_SOURCE        = "VerifyS";
const char* DrbdVolume::RS_LABEL_VERIFY_TARGET        = "VerifyT";
const char* DrbdVolume::RS_LABEL_AHEAD                = "Ahead";
const char* DrbdVolume::RS_LABEL_BEHIND               = "Behind";
const char* DrbdVolume::RS_LABEL_UNKNOWN              = "Unknown";

DrbdVolume::DrbdVolume(uint16_t volume_nr) :
    vol_nr(volume_nr)
{
    minor_nr = -1;
    vol_disk_state = DrbdVolume::disk_state::UNKNOWN;
    vol_repl_state = DrbdVolume::repl_state::UNKNOWN;
}

const uint16_t DrbdVolume::get_volume_nr() const
{
    return vol_nr;
}

// @throws EventMessageException
void DrbdVolume::update(PropsMap& event_props)
{
    std::string* prop_disk = event_props.get(&PROP_KEY_DISK);
    if (prop_disk == nullptr)
    {
        prop_disk = event_props.get(&PROP_KEY_PEER_DISK);
    }

    if (prop_disk != nullptr)
    {
        vol_disk_state = parse_disk_state(*prop_disk);
    }

    std::string* prop_replication = event_props.get(&PROP_KEY_REPLICATION);
    if (prop_replication != nullptr)
    {
        vol_repl_state = parse_repl_state(*prop_replication);
    }

    std::string* minor_nr_str = event_props.get(&PROP_KEY_MINOR);
    if (minor_nr_str != nullptr)
    {
        try
        {
            minor_nr = DrbdVolume::parse_minor_nr(*minor_nr_str);
        }
        catch (NumberFormatException& nf_exc)
        {
            throw EventMessageException();
        }
    }
}

int32_t DrbdVolume::get_minor_nr() const
{
    return minor_nr;
}

// @throws EventMessageException
void DrbdVolume::set_minor_nr(int32_t value)
{
    if (value >= -1 && value < 0x100000)
    {
        minor_nr = value;
    }
    else
    {
        throw EventMessageException();
    }
}

DrbdVolume::disk_state DrbdVolume::get_disk_state() const
{
    return vol_disk_state;
}

const char* DrbdVolume::get_disk_state_label() const
{
    const char* label = DS_LABEL_UNKNOWN;
    switch (vol_disk_state)
    {
        case DrbdVolume::disk_state::ATTACHING:
            label = DS_LABEL_ATTACHING;
            break;
        case DrbdVolume::disk_state::DETACHING:
            label = DS_LABEL_DETACHING;
            break;
        case DrbdVolume::disk_state::CONSISTENT:
            label = DS_LABEL_CONSISTENT;
            break;
        case DrbdVolume::disk_state::DISKLESS:
            label = DS_LABEL_DISKLESS;
            break;
        case DrbdVolume::disk_state::FAILED:
            label = DS_LABEL_FAILED;
            break;
        case DrbdVolume::disk_state::INCONSISTENT:
            label = DS_LABEL_INCONSISTENT;
            break;
        case DrbdVolume::disk_state::NEGOTIATING:
            label = DS_LABEL_NEGOTIATING;
            break;
        case DrbdVolume::disk_state::OUTDATED:
            label = DS_LABEL_OUTDATED;
            break;
        case DrbdVolume::disk_state::UP_TO_DATE:
            label = DS_LABEL_UP_TO_DATE;
            break;
        case DrbdVolume::disk_state::UNKNOWN:
            // fall-through
        default:
            break;
    }
    return label;
}

DrbdVolume::repl_state DrbdVolume::get_replication_state() const
{
    return vol_repl_state;
}

const char* DrbdVolume::get_replication_state_label() const
{
    const char* label = RS_LABEL_UNKNOWN;
    switch (vol_repl_state)
    {
        case DrbdVolume::repl_state::AHEAD:
            label = RS_LABEL_AHEAD;
            break;
        case DrbdVolume::repl_state::BEHIND:
            label = RS_LABEL_BEHIND;
            break;
        case DrbdVolume::repl_state::ESTABLISHED:
            label = RS_LABEL_ESTABLISHED;
            break;
        case DrbdVolume::repl_state::OFF:
            label = RS_LABEL_OFF;
            break;
        case DrbdVolume::repl_state::PAUSED_SYNC_SOURCE:
            label = RS_LABEL_PAUSED_SYNC_SOURCE;
            break;
        case DrbdVolume::repl_state::PAUSED_SYNC_TARGET:
            label = RS_LABEL_PAUSED_SYNC_TARGET;
            break;
        case DrbdVolume::repl_state::STARTING_SYNC_SOURCE:
            label = RS_LABEL_STARTING_SYNC_SOURCE;
            break;
        case DrbdVolume::repl_state::STARTING_SYNC_TARGET:
            label = RS_LABEL_STARTING_SYNC_TARGET;
            break;
        case DrbdVolume::repl_state::SYNC_SOURCE:
            label = RS_LABEL_SYNC_SOURCE;
            break;
        case DrbdVolume::repl_state::SYNC_TARGET:
            label = RS_LABEL_SYNC_TARGET;
            break;
        case DrbdVolume::repl_state::VERIFY_SOURCE:
            label = RS_LABEL_VERIFY_SOURCE;
            break;
        case DrbdVolume::repl_state::VERIFY_TARGET:
            label = RS_LABEL_VERIFY_TARGET;
            break;
        case DrbdVolume::repl_state::WF_BITMAP_SOURCE:
            label = RS_LABEL_WF_BITMAP_SOURCE;
            break;
        case DrbdVolume::repl_state::WF_BITMAP_TARGET:
            label = RS_LABEL_WF_BITMAP_TARGET;
            break;
        case DrbdVolume::repl_state::WF_SYNC_UUID:
            label = RS_LABEL_WF_SYNC_UUID;
            break;
        case DrbdVolume::repl_state::UNKNOWN:
            // fall-through
        default:
            break;
    }
    return label;
}

void DrbdVolume::set_connection(DrbdConnection* conn)
{
    connection = conn;
}

bool DrbdVolume::has_disk_alert()
{
    return disk_alert;
}

bool DrbdVolume::has_replication_warning()
{
    return repl_warn || repl_alert;
}

bool DrbdVolume::has_replication_alert()
{
    return repl_alert;
}

void DrbdVolume::clear_state_flags()
{
    disk_alert = false;
    repl_warn  = false;
    repl_alert = false;
    StateFlags::clear_state_flags();
}

StateFlags::state DrbdVolume::update_state_flags()
{
    // Reset the state to normal
    StateFlags::clear_state_flags();
    disk_alert = false;
    repl_warn  = false;
    repl_alert = false;

    // Check the volume's disk state
    switch (vol_disk_state)
    {
        case DrbdVolume::disk_state::UP_TO_DATE:
            // fall-through
        case DrbdVolume::disk_state::DISKLESS:
            // UpToDate disk or diskless client
            // no warning, no alert
            break;
        case DrbdVolume::disk_state::UNKNOWN:
            if (connection == nullptr)
            {
                // Volume is local, always issue an alert for
                // an unknown disk state
                disk_alert = true;
                set_alert();
            }
            else
            {
                // Volume is a peer volume, if the connection
                // to the peer is faulty, do not issue an alert for
                // an unknown disk state
                if (!connection->has_warn_state())
                {
                    disk_alert = true;
                    set_alert();
                }
            }

            break;
        case DrbdVolume::disk_state::ATTACHING:
            // fall-through
        case DrbdVolume::disk_state::CONSISTENT:
            // fall-through
        case DrbdVolume::disk_state::DETACHING:
            // fall-through
        case DrbdVolume::disk_state::FAILED:
            // fall-through
        case DrbdVolume::disk_state::INCONSISTENT:
            // fall-through
        case DrbdVolume::disk_state::NEGOTIATING:
            // fall-through
        case DrbdVolume::disk_state::OUTDATED:
            // fall-through
        default:
            disk_alert = true;
            set_alert();
            break;
    }

    // Check the volume's replication state
    switch (vol_repl_state)
    {
        case DrbdVolume::repl_state::ESTABLISHED:
            // no warning, no alert
            break;
        case DrbdVolume::repl_state::PAUSED_SYNC_SOURCE:
            // fall-through
        case DrbdVolume::repl_state::PAUSED_SYNC_TARGET:
            // fall-through
        case DrbdVolume::repl_state::STARTING_SYNC_SOURCE:
            // fall-through
        case DrbdVolume::repl_state::STARTING_SYNC_TARGET:
            // fall-through
        case DrbdVolume::repl_state::SYNC_SOURCE:
            // fall-through
        case DrbdVolume::repl_state::SYNC_TARGET:
            // fall-through
        case DrbdVolume::repl_state::VERIFY_SOURCE:
            // fall-through
        case DrbdVolume::repl_state::VERIFY_TARGET:
            // fall-through
        case DrbdVolume::repl_state::WF_BITMAP_SOURCE:
            // fall-through
        case DrbdVolume::repl_state::WF_BITMAP_TARGET:
            // fall-through
        case DrbdVolume::repl_state::WF_SYNC_UUID:
            repl_warn = true;
            set_warn();
            break;
        case DrbdVolume::repl_state::UNKNOWN:
            // fall-through
        case DrbdVolume::repl_state::OFF:
            if (connection != nullptr)
            {
                // Volume is a peer volume, if the connection
                // to the peer is faulty, do not issue alerts for
                // inoperative replication
                if (!connection->has_warn_state())
                {
                    repl_alert = true;
                    set_alert();
                }
            }
            // No alert is issued for local (non-peer) volumes,
            // because those always have 'Unknown' replication state
            break;
        case DrbdVolume::repl_state::AHEAD:
            // fall-through
        case DrbdVolume::repl_state::BEHIND:
            // fall-through
        default:
            repl_alert = true;
            set_alert();
            break;
    }

    return obj_state;
}

// @throws EventMessageException
DrbdVolume::disk_state DrbdVolume::parse_disk_state(std::string& state_name)
{
    DrbdVolume::disk_state state = DrbdVolume::disk_state::UNKNOWN;

    if (state_name == DS_LABEL_DISKLESS)
    {
        state = DrbdVolume::disk_state::DISKLESS;
    }
    else
    if (state_name == DS_LABEL_ATTACHING)
    {
        state = DrbdVolume::disk_state::ATTACHING;
    }
    else
    if (state_name == DS_LABEL_DETACHING)
    {
        state = DrbdVolume::disk_state::DETACHING;
    }
    else
    if (state_name == DS_LABEL_FAILED)
    {
        state = DrbdVolume::disk_state::FAILED;
    }
    else
    if (state_name == DS_LABEL_NEGOTIATING)
    {
        state = DrbdVolume::disk_state::NEGOTIATING;
    }
    else
    if (state_name == DS_LABEL_INCONSISTENT)
    {
        state = DrbdVolume::disk_state::INCONSISTENT;
    }
    else
    if (state_name == DS_LABEL_OUTDATED)
    {
        state = DrbdVolume::disk_state::OUTDATED;
    }
    else
    if (state_name == DS_LABEL_CONSISTENT)
    {
        state = DrbdVolume::disk_state::CONSISTENT;
    }
    else
    if (state_name == DS_LABEL_UP_TO_DATE)
    {
        state = DrbdVolume::disk_state::UP_TO_DATE;
    }
    else
    if (state_name != DS_LABEL_UNKNOWN)
    {
        throw EventMessageException();
    }

    return state;
}

// @throws EventMessageException
DrbdVolume::repl_state DrbdVolume::parse_repl_state(std::string& state_name)
{
    DrbdVolume::repl_state state = DrbdVolume::repl_state::UNKNOWN;

    if (state_name == RS_LABEL_AHEAD)
    {
        state = DrbdVolume::repl_state::AHEAD;
    }
    else
    if (state_name == RS_LABEL_BEHIND)
    {
        state = DrbdVolume::repl_state::BEHIND;
    }
    else
    if (state_name == RS_LABEL_ESTABLISHED)
    {
        state = DrbdVolume::repl_state::ESTABLISHED;
    }
    else
    if (state_name == RS_LABEL_OFF)
    {
        state = DrbdVolume::repl_state::OFF;
    }
    else
    if (state_name == RS_LABEL_PAUSED_SYNC_SOURCE)
    {
        state = DrbdVolume::repl_state::PAUSED_SYNC_SOURCE;
    }
    else
    if (state_name == RS_LABEL_PAUSED_SYNC_TARGET)
    {
        state = DrbdVolume::repl_state::PAUSED_SYNC_TARGET;
    }
    else
    if (state_name == RS_LABEL_STARTING_SYNC_SOURCE)
    {
        state = DrbdVolume::repl_state::STARTING_SYNC_SOURCE;
    }
    else
    if (state_name == RS_LABEL_STARTING_SYNC_TARGET)
    {
        state = DrbdVolume::repl_state::STARTING_SYNC_TARGET;
    }
    else
    if (state_name == RS_LABEL_SYNC_SOURCE)
    {
        state = DrbdVolume::repl_state::SYNC_SOURCE;
    }
    else
    if (state_name == RS_LABEL_SYNC_TARGET)
    {
        state = DrbdVolume::repl_state::SYNC_TARGET;
    }
    else
    if (state_name == RS_LABEL_VERIFY_SOURCE)
    {
        state = DrbdVolume::repl_state::VERIFY_SOURCE;
    }
    else
    if (state_name == RS_LABEL_VERIFY_TARGET)
    {
        state = DrbdVolume::repl_state::VERIFY_TARGET;
    }
    else
    if (state_name == RS_LABEL_WF_BITMAP_SOURCE)
    {
        state = DrbdVolume::repl_state::WF_BITMAP_SOURCE;
    }
    else
    if (state_name == RS_LABEL_WF_BITMAP_TARGET)
    {
        state = DrbdVolume::repl_state::WF_BITMAP_TARGET;
    }
    else
    if (state_name == RS_LABEL_WF_SYNC_UUID)
    {
        state = DrbdVolume::repl_state::WF_SYNC_UUID;
    }
    else
    if (state_name != RS_LABEL_UNKNOWN)
    {
        throw EventMessageException();
    }

    return state;
}

// @throws NumberFormatException
uint16_t DrbdVolume::parse_volume_nr(std::string& value_str)
{
    uint16_t value {0};
    value = NumberParser::parse_uint16(value_str);
    return value;
}

// @throws NumberFormatException
int32_t DrbdVolume::parse_minor_nr(std::string& value_str)
{
    int32_t value {-1};
    value = NumberParser::parse_int32(value_str);
    return value;
}

// Creates (allocates and initializes) a new DrbdVolume object from a map of properties
//
// @param event_props Reference to the map of properties from a 'drbdsetup events2' line
// @return Pointer to a newly created DrbdVolume object
// @throws std::bad_alloc, EventMessageException
DrbdVolume* DrbdVolume::new_from_props(PropsMap& event_props)
{
    DrbdVolume* vol {nullptr};
    std::string* number_str = event_props.get(&PROP_KEY_VOL_NR);
    if (number_str != nullptr)
    {
        try
        {
            uint16_t vol_nr = NumberParser::parse_uint16(*number_str);
            vol = new DrbdVolume(vol_nr);
        }
        catch (NumberFormatException& nf_exc)
        {
            // no-op
        }
    }
    if (vol == nullptr)
    {
        throw EventMessageException();
    }
    return vol;
}
