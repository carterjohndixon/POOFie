#include "db_handler.h"

// Connected to Test Cluster at 127.0.0.1:9042
// [cqlsh 6.1.0 | Cassandra 4.1.4 | CQL spec 3.4.6 | Native protocol v5]
// Use HELP for help.

namespace db
{
    db_handler::db_handler(const char *contact_points)
    {
        this->cluster = cass_cluster_new();
        this->session = cass_session_new();
        this->contact_points = contact_points;
        this->rc = new CassError;

        cass_cluster_set_contact_points(cluster, contact_points);

        CassFuture *connect_future = cass_session_connect(this->session, this->cluster);

        *this->rc = cass_future_error_code(connect_future);

        if (*this->rc != CASS_OK)
        {
            // SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "POOFie", "Couldn't connect to Database", NULL);
        }
    }

    db_handler::~db_handler()
    {
        cass_cluster_free(this->cluster);
        cass_session_free(this->session);
    }

    bool db_handler::check_db_contact_point()
    {
        if (*this->rc != CASS_OK)
            return false;
        else
            return true;
    }
}