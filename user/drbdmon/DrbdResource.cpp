#include <DrbdResource.h>
#include <comparators.h>

const std::string DrbdResource::PROP_KEY_RES_NAME = "name";

// @throws std::bad_alloc
DrbdResource::DrbdResource(std::string& resource_name):
    name(resource_name)
{
    conn_list = new ConnectionsMap(&comparators::compare_string);
}

DrbdResource::~DrbdResource() noexcept
{
    ConnectionsMap::NodesIterator dtor_iter(*conn_list);
    while (dtor_iter.has_next())
    {
        ConnectionsMap::Node* node = dtor_iter.next();
        delete node->get_key();
        delete node->get_value();
    }
    conn_list->clear();
    delete conn_list;
}

const std::string& DrbdResource::get_name() const
{
    return name;
}

// @throws EventMessageException
void DrbdResource::update(PropsMap& event_props)
{
    std::string* prop_role = event_props.get(&PROP_KEY_ROLE);
    if (prop_role != nullptr)
    {
        role = parse_role(*prop_role);
    }
}

// @throws std::bad_alloc, dsaext::DuplicateInsertException
void DrbdResource::add_connection(DrbdConnection* connection)
{
    std::string* conn_name {nullptr};
    try
    {
        conn_name = new std::string(connection->get_name());
        conn_list->insert(conn_name, connection);
    }
    catch (dsaext::DuplicateInsertException& dup_exc)
    {
        delete conn_name;
        throw dup_exc;
    }
    catch (std::bad_alloc& out_of_memory_exc)
    {
        delete conn_name;
        throw out_of_memory_exc;
    }
}

DrbdConnection* DrbdResource::get_connection(const std::string& connection_name) const
{
    DrbdConnection* connection = conn_list->get(&connection_name);
    return connection;
}

void DrbdResource::remove_connection(const std::string& connection_name)
{
    ConnectionsMap::Node* node = conn_list->get_node(&connection_name);
    delete node->get_key();
    delete node->get_value();
    conn_list->remove_node(node);
}

DrbdResource::ConnectionsIterator DrbdResource::connections_iterator()
{
    return ConnectionsIterator(*this);
}

bool DrbdResource::has_role_alert()
{
    return role_alert;
}

void DrbdResource::clear_state_flags()
{
    role_alert = false;
    StateFlags::clear_state_flags();
}

StateFlags::state DrbdResource::update_state_flags()
{
    // Reset the state to normal
    StateFlags::clear_state_flags();
    role_alert = false;

    // Alert for unknown resource role
    switch (role)
    {
        case DrbdRole::resource_role::PRIMARY:
            // fall-through
        case DrbdRole::resource_role::SECONDARY:
            // no warning, no alert
            break;
        case DrbdRole::resource_role::UNKNOWN:
            // fall-through
        default:
            role_alert = true;
            set_alert();
            break;
    }

    // Check the resource's volumes
    VolumesMap::ValuesIterator vol_iter = volumes_iterator();
    size_t vol_count = vol_iter.get_size();
    for (size_t vol_index = 0; vol_index < vol_count; ++vol_index)
    {
        DrbdVolume& vol = *(vol_iter.next());
        if (vol.update_state_flags() != StateFlags::state::NORM)
        {
            set_mark();
        }
    }

    // Check the resource's connections
    ConnectionsMap::ValuesIterator conn_iter = connections_iterator();
    size_t conn_count = conn_iter.get_size();
    for (size_t conn_index = 0; conn_index < conn_count; ++conn_index)
    {
        DrbdConnection& conn = *(conn_iter.next());
        if (conn.update_state_flags() != StateFlags::state::NORM)
        {
            // Connections are in an abnormal state, mark this resource
            set_mark();
        }
    }

    return obj_state;
}

// Creates (allocates and initializes) a new DrbdResource object from a map of properties
//
// @param event_props Reference to the map of properties from a 'drbdsetup events2' line
// @return Pointer to a newly created DrbdResource object
// @throws std::bad_alloc, dsaext::DuplicateInsertException
DrbdResource* DrbdResource::new_from_props(PropsMap& event_props)
{
    DrbdResource* new_res {nullptr};
    std::string* res_name = event_props.get(&PROP_KEY_RES_NAME);
    if (res_name != nullptr)
    {
        new_res = new DrbdResource(*res_name);
    }
    if (new_res == nullptr)
    {
        throw EventMessageException();
    }
    return new_res;
}
