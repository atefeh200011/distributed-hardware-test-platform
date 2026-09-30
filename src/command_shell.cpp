#include "command_shell.h"

#include <chrono>
#include <filesystem>
#include <sstream>
#include <string>
#include <cctype>

#include "test_executor.h"
#include "test_procedure_json.h"
#include "test_report_json.h"

namespace
{
std::string trim_copy(const std::string& text)
{
    const std::size_t first =
        text.find_first_not_of(" \t\r\n");

    if (first == std::string::npos)
    {
        return {};
    }

    const std::size_t last =
        text.find_last_not_of(" \t\r\n");

    return text.substr(
        first,
        last - first + 1);
}

bool begins_with_keyword(
    const std::string& text,
    const std::string& keyword)
{
    if (text.size() <= keyword.size())
    {
        return false;
    }

    if (text.compare(
            0,
            keyword.size(),
            keyword) != 0)
    {
        return false;
    }

    return std::isspace(
        static_cast<unsigned char>(
            text[keyword.size()])) != 0;
}

bool parse_legacy_relay_action(
    const std::string& command,
    std::string& action)
{
    std::istringstream command_stream(command);

    std::string relay_word;
    std::string extra_argument;

    command_stream
        >> relay_word
        >> action;

    if (relay_word != "relay" ||
        action.empty() ||
        command_stream >> extra_argument)
    {
        return false;
    }

    return true;
}
std::string create_report_path(
    const std::string& procedure_file)
{
    const auto timestamp =
        std::chrono::duration_cast<
            std::chrono::milliseconds>(
            std::chrono::system_clock::now()
                .time_since_epoch())
                .count();

    const std::filesystem::path procedure_path(
        procedure_file);

    const std::string report_name =
        procedure_path.stem().string() +
        "-" +
        std::to_string(timestamp) +
        ".json";

    return (
        std::filesystem::path("reports") /
        report_name).string();
}

template <typename RelaySource>
bool execute_procedure_file(
    const std::string& file_path,
    RelaySource& relay_source,
    std::ostream& output)
{
    TestProcedure procedure;
    std::string error_message;

    if (load_test_procedure_file(
            file_path,
            procedure,
            error_message) == false)
    {
        output
            << "Failed to load procedure: "
            << error_message
            << '\n';
        return true;
    }

    const TestResult result =
        execute_procedure(
            procedure,
            relay_source,
            output);

    const std::string report_path =
        create_report_path(file_path);

    if (write_test_report_file(
            report_path,
            result,
            error_message) == false)
    {
        output
            << "Failed to write report: "
            << error_message
            << '\n';
        return true;
    }

    output
        << "Report written: "
        << report_path
        << '\n';

    return true;
}

bool handle_named_relay_command(
    const std::string& command,
    RelayRegistry& relays,
    std::ostream& output)
{
    std::istringstream command_stream(command);

    std::string relay_word;
    std::string relay_name;
    std::string action;
    std::string extra_argument;

    command_stream
        >> relay_word
        >> relay_name
        >> action;

    if (relay_word != "relay" ||
        relay_name.empty() ||
        action.empty() ||
        command_stream >> extra_argument)
    {
        return false;
    }

    IRelay* relay = relays.find(relay_name);

    if (relay == nullptr)
    {
        output
            << "Relay not found: "
            << relay_name
            << '\n';
        return true;
    }

    if (action == "on")
    {
        relay->turn_on();

        output
            << "Relay "
            << relay_name
            << " state: on\n";

        return true;
    }

    if (action == "off")
    {
        relay->turn_off();

        output
            << "Relay "
            << relay_name
            << " state: off\n";

        return true;
    }

    if (action == "status")
    {
        output
            << "Relay "
            << relay_name
            << " state: "
            << (relay->is_on() ? "on" : "off")
            << '\n';

        return true;
    }

    return false;
}
}

void print_help(std::ostream& output)
{
    output << "Available commands:\n";
    output << "  help                    Show available commands\n";
    output << "  status                  Show platform status\n";
    output << "  relays                  List registered relays\n";
    output << "  relay <name> on         Switch a relay on\n";
    output << "  relay <name> off        Switch a relay off\n";
    output << "  relay <name> status     Show a relay state\n";
    output << "  run <file>              Run a JSON test procedure\n";
    output << "  exit                    Exit the application\n";
}

bool handle_command(
    const std::string& command,
    RelayRegistry& relays,
    std::ostream& output)
{
    const std::string normalized_command =
        trim_copy(command);

    if (normalized_command == "exit")
    {
        output
            << "Shutting down the hardware test platform project.\n";
        return false;
    }

    if (normalized_command == "help")
    {
        print_help(output);
        return true;
    }

    if (normalized_command == "status")
    {
        output << "Platform status: ready\n";
        return true;
    }

    if (normalized_command == "relays")
    {
        output << "Available relays:\n";

        for (const std::string& name : relays.names())
        {
            output << "  " << name << '\n';
        }

        return true;
    }

    if (begins_with_keyword(
            normalized_command,
            "relay"))
    {
        if (handle_named_relay_command(
                normalized_command,
                relays,
                output))
        {
            return true;
        }

        output
            << "Unknown command: "
            << normalized_command
            << '\n';
        return true;
    }

    if (begins_with_keyword(
            normalized_command,
            "run"))
    {
        const std::string file_path =
            trim_copy(
                normalized_command.substr(3));

        return execute_procedure_file(
            file_path,
            relays,
            output);
    }

    output
        << "Unknown command: "
        << normalized_command
        << '\n';
    return true;
}

bool handle_command(
    const std::string& command,
    IRelay& relay,
    std::ostream& output)
{
    const std::string normalized_command =
        trim_copy(command);

    if (normalized_command == "exit")
    {
        output
            << "Shutting down the hardware test platform project.\n";
        return false;
    }

    if (normalized_command == "help")
    {
        print_help(output);
        return true;
    }

    if (normalized_command == "status")
    {
        output << "Platform status: ready\n";
        return true;
    }

    std::string relay_action;

    if (parse_legacy_relay_action(
            normalized_command,
            relay_action))
    {
        if (relay_action == "on")
        {
            relay.turn_on();
            output << "Relay state: on\n";
            return true;
        }

        if (relay_action == "off")
        {
            relay.turn_off();
            output << "Relay state: off\n";
            return true;
        }

        if (relay_action == "status")
        {
            output
                << "Relay state: "
                << (relay.is_on() ? "on" : "off")
                << '\n';

            return true;
        }
    }

    if (begins_with_keyword(
            normalized_command,
            "run"))
    {
        const std::string file_path =
            trim_copy(
                normalized_command.substr(3));

        return execute_procedure_file(
            file_path,
            relay,
            output);
    }

    output
        << "Unknown command: "
        << normalized_command
        << '\n';
    return true;
}
