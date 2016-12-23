#ifndef DRBDVOLUME_H
#define	DRBDVOLUME_H

#include <new>
#include <cstdint>
#include <string>

#include <StateFlags.h>

#include <map_types.h>
// https://github.com/raltnoeder/cppdsaext
#include <dsaext.h>
#include <utils.h>
#include <exceptions.h>

class DrbdConnection;

class DrbdVolume : private StateFlags
{
  public:
    enum class disk_state : uint16_t
    {
        DISKLESS,
        ATTACHING,
        DETACHING,
        FAILED,
        NEGOTIATING,
        INCONSISTENT,
        OUTDATED,
        UNKNOWN,
        CONSISTENT,
        UP_TO_DATE
    };

    enum class repl_state : uint16_t
    {
        OFF,
        ESTABLISHED,
        STARTING_SYNC_SOURCE,
        STARTING_SYNC_TARGET,
        WF_BITMAP_SOURCE,
        WF_BITMAP_TARGET,
        WF_SYNC_UUID,
        SYNC_SOURCE,
        SYNC_TARGET,
        PAUSED_SYNC_SOURCE,
        PAUSED_SYNC_TARGET,
        VERIFY_SOURCE,
        VERIFY_TARGET,
        AHEAD,
        BEHIND,
        UNKNOWN
    };

    static const std::string PROP_KEY_VOL_NR;
    static const std::string PROP_KEY_MINOR;
    static const std::string PROP_KEY_DISK;
    static const std::string PROP_KEY_PEER_DISK;
    static const std::string PROP_KEY_REPLICATION;

    static const char* DS_LABEL_DISKLESS;
    static const char* DS_LABEL_ATTACHING;
    static const char* DS_LABEL_DETACHING;
    static const char* DS_LABEL_FAILED;
    static const char* DS_LABEL_NEGOTIATING;
    static const char* DS_LABEL_INCONSISTENT;
    static const char* DS_LABEL_OUTDATED;
    static const char* DS_LABEL_UNKNOWN;
    static const char* DS_LABEL_CONSISTENT;
    static const char* DS_LABEL_UP_TO_DATE;

    static const char* RS_LABEL_OFF;
    static const char* RS_LABEL_ESTABLISHED;
    static const char* RS_LABEL_STARTING_SYNC_SOURCE;
    static const char* RS_LABEL_STARTING_SYNC_TARGET;
    static const char* RS_LABEL_WF_BITMAP_SOURCE;
    static const char* RS_LABEL_WF_BITMAP_TARGET;
    static const char* RS_LABEL_WF_SYNC_UUID;
    static const char* RS_LABEL_SYNC_SOURCE;
    static const char* RS_LABEL_SYNC_TARGET;
    static const char* RS_LABEL_PAUSED_SYNC_SOURCE;
    static const char* RS_LABEL_PAUSED_SYNC_TARGET;
    static const char* RS_LABEL_VERIFY_SOURCE;
    static const char* RS_LABEL_VERIFY_TARGET;
    static const char* RS_LABEL_AHEAD;
    static const char* RS_LABEL_BEHIND;
    static const char* RS_LABEL_UNKNOWN;

    explicit DrbdVolume(uint16_t volume_nr);
    DrbdVolume(const DrbdVolume& orig) = delete;
    DrbdVolume& operator=(const DrbdVolume& orig) = delete;
    DrbdVolume(DrbdVolume&& orig) = delete;
    DrbdVolume& operator=(DrbdVolume&& orig) = delete;
    virtual ~DrbdVolume() noexcept override
    {
    }

    virtual const uint16_t get_volume_nr() const;
    virtual int32_t get_minor_nr() const;

    // @throws EventMessageException
    virtual void set_minor_nr(int32_t value);

    // @throws EventMessageException
    virtual void update(PropsMap& event_props);

    virtual disk_state get_disk_state() const;
    virtual const char* get_disk_state_label() const;
    virtual repl_state get_replication_state() const;
    virtual const char* get_replication_state_label() const;
    virtual void set_connection(DrbdConnection* conn);

    using StateFlags::has_mark_state;
    using StateFlags::has_warn_state;
    using StateFlags::has_alert_state;
    using StateFlags::set_mark;
    using StateFlags::set_warn;
    using StateFlags::set_alert;
    using StateFlags::get_state;
    virtual void clear_state_flags() override;
    virtual StateFlags::state update_state_flags() override;
    virtual bool has_disk_alert();
    virtual bool has_replication_warning();
    virtual bool has_replication_alert();

    // Creates (allocates and initializes) a new DrbdVolume object from a map of properties
    //
    // @param event_props Reference to the map of properties from a 'drbdsetup events2' line
    // @return Pointer to a newly created DrbdVolume object
    // @throws std::bad_alloc, EventMessageException
    static DrbdVolume* new_from_props(PropsMap& event_props);

    // @throws EventMessageException
    static disk_state parse_disk_state(std::string& state_name);

    // @throws EventMessageException
    static repl_state parse_repl_state(std::string& state_name);

    // @throws NumberFormatException
    static uint16_t parse_volume_nr(std::string& value_str);

    // @throws NumberFormatException
    static int32_t parse_minor_nr(std::string& value_str);

  private:
    const uint16_t vol_nr;
    int32_t minor_nr {-1};
    disk_state vol_disk_state;
    repl_state vol_repl_state;
    DrbdConnection* connection {nullptr};
    bool disk_alert {false};
    bool repl_warn  {false};
    bool repl_alert {false};
};

#endif	/* DRBDVOLUME_H */
