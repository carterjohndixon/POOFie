#include "db_handler.h"

#include <iostream>
#include <stdio.h>

#include "../globals.h"
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
    // db_handler::db_handler(const char *contact_points)
    db_handler::db_handler(const char *contact_points, const char *username, const char *password)
    {
        std::cout << "Contact point\n";
        this->cluster = cass_cluster_new();
        this->session = cass_session_new();
        this->contact_points = contact_points;
        this->rc = new CassError;

        cass_cluster_set_contact_points(cluster, contact_points);

        cass_cluster_set_credentials(cluster, username, password);

        CassFuture *connect_future = cass_session_connect(this->session, this->cluster);

        *this->rc = cass_future_error_code(connect_future);

        if (*this->rc != CASS_OK)
        {
            // SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "POOFie", "Couldn't connect to Database", NULL);
        }

        cass_future_free(connect_future);

        const char *query = "SELECT item_count FROM store.shopping_cart";
        CassStatement *statement = cass_statement_new(query, 0);

        CassFuture *result_future = cass_session_execute(this->session, statement);
        cass_future_wait(result_future);

        *this->rc = cass_future_error_code(result_future);
        if (*this->rc != CASS_OK)
        {
            std::cerr << "Query execution failed\n";
            cass_future_free(result_future);
            cass_statement_free(statement);
            return;
        }

        const CassResult *result = cass_future_get_result(result_future);
        CassIterator *rows = cass_iterator_from_result(result);

        while (cass_iterator_next(rows))
        {
            const CassRow *row = cass_iterator_get_row(rows);
            const CassValue *value = cass_row_get_column_by_name(row, "item_count");

            cass_int32_t item_count;
            cass_value_get_int32(value, &item_count);

            std::cout << "Item Count: " << item_count << std::endl;
        }

        cass_iterator_free(rows);
        cass_result_free(result);
        cass_future_free(result_future);
        cass_statement_free(statement);
    }

    db_handler::db_handler(const char *client_id, const char *client_secret, const std::string *file_path)
    {
        std::cout << "Datastax\n";
        this->cluster = cass_cluster_new();
        this->session = cass_session_new();
        this->rc = new CassError;

        const char *secure_connect_bundle = file_path->c_str();
        if (cass_cluster_set_cloud_secure_connection_bundle(this->cluster, secure_connect_bundle) != CASS_OK)
        {
            fprintf(stderr, "Unable to configure cloud using the secure connection bundle: %s", secure_connect_bundle);
            globals.ConnectionErr = true;
            globals.ConnectionErrStr = "Invalid zip file";
        }
        else
        {
            globals.ConnectionErr = false;
        }

        cass_cluster_set_credentials(this->cluster, client_id, client_secret);

        CassFuture *connect_future = cass_session_connect(this->session, this->cluster);
        if (cass_future_error_code(connect_future) != CASS_OK)
        {
            const char *message;
            size_t message_length;
            cass_future_error_message(connect_future, &message, &message_length);
            fprintf(stderr, "Unable to establish connection: '%.*s'\n", (int)message_length, message);
            // Handle error
            cass_future_free(connect_future);
            return;
        }

        cass_future_free(connect_future);

        // Query execution logic
        const char *query = "SELECT item_count FROM store.shopping_cart";
        CassStatement *statement = cass_statement_new(query, 0);
        CassFuture *result_future = cass_session_execute(this->session, statement);
        cass_future_wait(result_future);

        if (cass_future_error_code(result_future) == CASS_OK)
        {
            const CassResult *result = cass_future_get_result(result_future);
            CassIterator *rows = cass_iterator_from_result(result);

            while (cass_iterator_next(rows))
            {
                const CassRow *row = cass_iterator_get_row(rows);
                const CassValue *value = cass_row_get_column_by_name(row, "item_count");

                cass_int32_t item_count;
                cass_value_get_int32(value, &item_count);

                std::cout << "Item Count: " << item_count << std::endl;
            }

            cass_iterator_free(rows);
            cass_result_free(result);
        }
        else
        {
            const char *message;
            size_t message_length;
            cass_future_error_message(result_future, &message, &message_length);
            std::cerr << "Unable to run query: " << std::string(message, message_length) << std::endl;
        }

        cass_statement_free(statement);
        cass_future_free(result_future);
    }

    db_handler::~db_handler()
    {
        cass_cluster_free(this->cluster);
        cass_session_free(this->session);

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