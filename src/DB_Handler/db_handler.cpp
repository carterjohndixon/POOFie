#include "db_handler.h"

#include <iostream>
#include <stdio.h>

// Connected to Test Cluster at 127.0.0.1:9042
// [cqlsh 6.1.0 | Cassandra 4.1.4 | CQL spec 3.4.6 | Native protocol v5]
// Use HELP for help.

// {
//   "clientId": "htONvmHWuswBjtLzARUBakSP",
//   "secret": "MNAWAPrkkgslzY1sO9J2Nw,yxKK7qpZMJ_osYaz84EL+7vRHaoQcXO_mnkiIx15,f18i.lxT,3gHcWjIOu6tOwZwNwuEWjbjR_+0RWQWf_w+hwycPcIJk15OvW.FoRvX",
//   "token": "AstraCS:htONvmHWuswBjtLzARUBakSP:786da047e19b5fa10216f2b76b071316c5e453e69b226b00cdbb7a9b8bd53f2c"
// }

namespace db
{
    db_handler::db_handler(const char *contact_points)
    // db_handler::db_handler(const char *username, const char *password)
    {
        this->cluster = cass_cluster_new();
        this->session = cass_session_new();
        this->contact_points = contact_points;
        this->rc = new CassError;

        cass_cluster_set_contact_points(cluster, contact_points);

        // const char *username = "cass_ui_dev";
        // const char *password = "cassandra_gui";

        // cass_cluster_set_credentials(cluster, username, password);

        CassFuture *connect_future = cass_session_connect(this->session, this->cluster);

        // cass_cluster_set_client_id(this->cluster, cass_uuid_gen_new())

        *this->rc = cass_future_error_code(connect_future);

        if (*this->rc != CASS_OK)
        {
            // SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "POOFie", "Couldn't connect to Database", NULL);
        }

        cass_future_free(connect_future);
        cass_session_free(this->session);
        cass_cluster_free(this->cluster);
    }

    // db_handler::db_handler(const char *username, const char *password)
    // {
    //     this->cluster = cass_cluster_new();
    //     this->session = cass_session_new();
    //     this->contact_points = contact_points;
    //     this->rc = new CassError;

    //     // cass_cluster_set_credentials(cluster, "cass_ui_dev", "cassandra_gui");
    //     cass_session_connect_keyspace(session, cluster, "store");

    //     cass_cluster_set_connect_timeout(cluster, 10000);

    //     CassFuture *connect_future = cass_session_connect(session, cluster);

    //     *this->rc = cass_future_error_code(connect_future);

    //     if (*this->rc == CASS_OK)
    //     {

    //         std::cout << "Successfully connected to Cassandra\n";

    //         const char *query = "SELECT release_version FROM system.local";
    //         CassStatement *statement = cass_statement_new(query, 0);

    //         CassFuture *result_future = cass_session_execute(session, statement);

    //         if (cass_future_error_code(result_future) == CASS_OK)
    //         {
    //             /* Retrieve result set and get the first row */
    //             const CassResult *result = cass_future_get_result(result_future);
    //             const CassRow *row = cass_result_first_row(result);

    //             if (row)
    //             {
    //                 const CassValue *value = cass_row_get_column_by_name(row, "release_version");

    //                 const char *release_version;
    //                 size_t release_version_length;
    //                 cass_value_get_string(value, &release_version, &release_version_length);
    //                 printf("release_version: '%.*s'", (int)release_version_length, release_version);
    //             }

    //             cass_result_free(result);
    //         }
    //         else
    //         {
    //         }

    //         cass_statement_free(statement);
    //         cass_future_free(result_future);
    //     }
    //     else
    //     {
    //         std::cout << "Couldn't connected to Cassandra\n";
    //     }

    //     cass_future_free(connect_future);
    //     cass_cluster_free(this->cluster);
    //     cass_session_free(this->session);
    // }

    db_handler::~db_handler()
    {
        // cass_cluster_free(this->cluster);
        // cass_session_free(this->session);

        delete this->rc;
    }

    bool db_handler::is_connected()
    {
        if (*this->rc != CASS_OK)
            return false;
        else
            return true;
    }
}