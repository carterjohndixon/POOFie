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

        delete this->rc;
    }

    bool db_handler::is_connected()
    {
        // return (*this->rc == CASS_OK);
        if (*this->rc != CASS_OK)
            return false;
        else
            return true;
    }
}

// if (ImGui::Button("Login", ImVec2(267, 40)))
// {
//     // Start the spinner before initiating the database connection
//     connecting = true;

//     // Initiate the database connection in a separate thread
// std::thread db_connection_thread([&contact_point, &good_contact, &screenW]()
//                                  {
//             gui::connect_to_db(contact_point);
//             if (gui::check_db_connection())
//             {
//                 std::cout << "good connect\n";
//                 *good_contact = true;
//                 globals.login_loading = true;
//                 globals.main_form = true;
//                 globals.login_form = false;
//             }
//             else
//             {
//                 std::cout << "bad connect\n";
//                 ImGui::PushFont(globals.verdana);
//                 ImGui::SetCursorPos(ImVec2((*screenW / 2) - IM_ARRAYSIZE(globals.contact_point), 175));
//                 ImGui::TextColored(ImVec4(1.0f, 0.f, 0.f, 1.f), "Invalid contact point!");
//                 ImGui::PopFont();
//                 connection_data.invalid_contact = false;
//             }

//     // Once the database connection process is done, stop the spinner
//     connecting = false; });

//     // Detach the thread so it can run independently
//     db_connection_thread.detach();
// }