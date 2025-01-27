#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "MainComponent.h"

class AudioServerApplication final : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return JUCE_APPLICATION_NAME_STRING; }

    const juce::String getApplicationVersion() override { return JUCE_APPLICATION_VERSION_STRING; }

    void initialise(const juce::String &commandLineParameters) override
    {
        const auto exe{juce::File::getSpecialLocation(juce::File::currentExecutableFile)};

        juce::StringArray args;
        args.addTokens(commandLineParameters, true);
        args.trim();

        for (auto &s: args)
            s = s.unquoted();

        juce::ArgumentList argList(exe.getFullPathName(), args);

        juce::ConsoleApplication cli;
        cli.addHelpCommand("--help|-h", "Usage:", true);
        cli.addVersionCommand("--version|-v", juce::String{JUCE_APPLICATION_NAME_STRING " " JUCE_APPLICATION_VERSION_STRING});
        cli.addCommand({
            "--file|-f",
            "--file|-f",
            "Plays back the given audio (.wav, .aif) file",
            juce::String{},
            [this](const juce::ArgumentList &a)
            {
                const auto file{juce::File(a.getFileForOption("--file|-f"))};
                mainComponent = std::make_unique<MainComponent>(file);
            }
        });

        if (const auto retValue = cli.findAndRunCommand(argList); retValue != 0) {
            getInstance()->setApplicationReturnValue(retValue);
        }

        // quit();
    }

    void shutdown() override
    {
    }

private:
    std::unique_ptr<MainComponent> mainComponent;
};

START_JUCE_APPLICATION(AudioServerApplication);
