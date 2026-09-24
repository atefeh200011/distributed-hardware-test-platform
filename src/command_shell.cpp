#include "test_executor.h"
#include "test_procedure_json.h"
#include "command_shell.h"
#include "test_report_json.h"

#include <chrono>
#include <filesystem>

namespace
{
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
}

void print_help(std::ostream& output)
{
    output << "Available commands:\n";
    output << "  help    Show available commands\n";
    output << "  status  Show platform status\n";
    output << "  relay on      Switch the relay on\n";
    output << "  relay off     Switch the relay off\n";
    output << "  relay status  Show the relay state\n";
    output << "  exit    Exit the application\n";
    output << "  run <file>    Run a JSON test procedure\n";
}

bool handle_command(
    std::string command,
    IRelay& relay,
    std::ostream& output)
{
    if (command == "exit")
    {
        output << "Shutting down the hardware test platform project.\n";
        return false;
    }
    else if (command == "help")
    {
        print_help(output);
        return true;
    }
    else if (command == "relay on")
    {
        relay.turn_on();
        output << "Relay state: on\n";
        return true;
    }
    else if (command == "relay off")
    {
        relay.turn_off();
        output << "Relay state: off\n";
        return true;
    }
    else if (command == "relay status")
 {
        if (relay.is_on() == true)
        {
            output << "Relay state: on\n";
        }
        else
        {
            output << "Relay state: off\n";
        }

        return true;
    }

    else if (command.starts_with("run "))
    {
        const std::string file_path =
            command.substr(4);

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
                relay,
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
    else if (command == "status")
    {
        output << "Platform status: ready\n";
        return true;
    }
    else
    {
        output << "Unknown command: " << command << '\n';
        return true;
    }
}


